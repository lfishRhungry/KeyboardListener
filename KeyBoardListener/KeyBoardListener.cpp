#include "KeyBoardListener.h"
#include "VirtualKeyToAscii.h"

KeyBoardListener::KeyBoardListener(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//-------------------------------�������------------------------------
	this->setMinimumSize(1200, 1000);
	this->setWindowTitle("Keyboard Listener");
	ui.buttonStart->setText("Start listening");
	ui.buttonExit->setText("Exit");
	ui.plainTextEdit->setReadOnly(true);
	//��Ӻ��������
	//ui.plainTextEdit->setWordWrapMode(QTextOption::NoWrap);
	//-------------------------------������ƽ���------------------------------

	hWnd = (HWND)winId(); // ��ȡ�����򴰿ھ��

	// ��ʼ����
	connect(ui.buttonStart, &QPushButton::clicked, [=] {
		ui.buttonStart->setEnabled(false);
		Register(); // ע�ἴ��ʼ����
		});

	// �رճ���
	connect(ui.buttonExit, &QPushButton::clicked, this, &KeyBoardListener::close);
}

// �������
void KeyBoardListener::ShowError(PTCHAR pszText) {
	TCHAR szErr[MAX_PATH] = { 0 };
	::wsprintf(szErr, TEXT("%s Error[%d]\n"), pszText, ::GetLastError());
	QMessageBox::critical(this, "ERROR", QString::fromWCharArray(szErr));
}

// ע��ԭʼ�豸
BOOL KeyBoardListener::Register() {
	// ���� RAWINPUTDEVICE �ṹ����Ϣ ָ��Ҫ��������
	RAWINPUTDEVICE rawinputDevice = { 0 };
	rawinputDevice.usUsagePage = 0x01;
	rawinputDevice.usUsage = 0x06;
	rawinputDevice.dwFlags = RIDEV_INPUTSINK; // ��ʹ�������Ƕ��㴰��ҲҪ����
	rawinputDevice.hwndTarget = hWnd;

	// ע��ԭʼ�����豸
	BOOL bRet = ::RegisterRawInputDevices(&rawinputDevice, 1, sizeof(rawinputDevice));
	if (FALSE == bRet)
	{
		ShowError(TEXT("RegisterRawInputDevices"));
		return FALSE;
	}

	return TRUE;
}

// ��ȡԭʼ��������
BOOL KeyBoardListener::GetData(LPARAM lParam) {
	RAWINPUT rawinputData = { 0 };
	UINT uiSize = sizeof(rawinputData);

	// ��ȡԭʼ�������ݵĴ�С
	::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawinputData, &uiSize, sizeof(RAWINPUTHEADER));
	if (RIM_TYPEKEYBOARD == rawinputData.header.dwType)
	{
		// WM_KEYDOWN --> ��ͨ����    WM_SYSKEYDOWN --> ϵͳ����(ָ����ALT)  
		if ((WM_KEYDOWN == rawinputData.data.keyboard.Message) ||
			(WM_SYSKEYDOWN == rawinputData.data.keyboard.Message))
		{
			// ��¼����
			SaveKey(rawinputData.data.keyboard.VKey);
		}
	}
	return TRUE;
}

// ���水����Ϣ
void KeyBoardListener::SaveKey(USHORT usVKey) {
	TCHAR szKey[MAX_PATH] = { 0 };
	TCHAR szTitle[MAX_PATH] = { 0 };
	TCHAR szText[MAX_PATH] = { 0 };
	// ��ȡ���㴰��
	HWND hForegroundWnd = ::GetForegroundWindow();
	// ��ȡ���㴰�ڱ���
	::GetWindowText(hForegroundWnd, szTitle, 256);
	// ���������ת���ɶ�Ӧ��ASCII
	::lstrcpy(szKey, GetKeyName(usVKey));
	
	
	// ���찴����¼��Ϣ�ַ���
	::wsprintf(szText, TEXT("[%s] %s\r\n"), szTitle, szKey);
	
	// �������
	ui.plainTextEdit->appendPlainText(QString::fromWCharArray(szText));
}

// (����)��Ϣ������
bool KeyBoardListener::nativeEvent(const QByteArray& eventType, void* message, long* result) {
	MSG* msg = (MSG*)message;
	if (msg->message == WM_INPUT) {
		GetData(msg->lParam);
	}

	return false;
}
