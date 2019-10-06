#include "KeyBoardListener.h"
#include "VirtualKeyToAscii.h"

KeyBoardListener::KeyBoardListener(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//-------------------------------界面设计------------------------------
	this->setMinimumSize(1200, 1000);
	this->setWindowTitle("Keyboard Listener");
	ui.buttonStart->setText("Start listening");
	ui.buttonExit->setText("Exit");
	ui.plainTextEdit->setReadOnly(true);
	//添加横向滚动条
	//ui.plainTextEdit->setWordWrapMode(QTextOption::NoWrap);
	//-------------------------------界面设计结束------------------------------

	hWnd = (HWND)winId(); // 获取本程序窗口句柄

	// 开始监听
	connect(ui.buttonStart, &QPushButton::clicked, [=] {
		ui.buttonStart->setEnabled(false);
		Register(); // 注册即开始监听
		});

	// 关闭程序
	connect(ui.buttonExit, &QPushButton::clicked, this, &KeyBoardListener::close);
}

// 处理错误
void KeyBoardListener::ShowError(PTCHAR pszText) {
	TCHAR szErr[MAX_PATH] = { 0 };
	::wsprintf(szErr, TEXT("%s Error[%d]\n"), pszText, ::GetLastError());
	QMessageBox::critical(this, "ERROR", QString::fromWCharArray(szErr));
}

// 注册原始设备
BOOL KeyBoardListener::Register() {
	// 设置 RAWINPUTDEVICE 结构体信息 指定要监听键盘
	RAWINPUTDEVICE rawinputDevice = { 0 };
	rawinputDevice.usUsagePage = 0x01;
	rawinputDevice.usUsage = 0x06;
	rawinputDevice.dwFlags = RIDEV_INPUTSINK; // 即使本程序不是顶层窗口也要监听
	rawinputDevice.hwndTarget = hWnd;

	// 注册原始输入设备
	BOOL bRet = ::RegisterRawInputDevices(&rawinputDevice, 1, sizeof(rawinputDevice));
	if (FALSE == bRet)
	{
		ShowError(TEXT("RegisterRawInputDevices"));
		return FALSE;
	}

	return TRUE;
}

// 获取原始输入数据
BOOL KeyBoardListener::GetData(LPARAM lParam) {
	RAWINPUT rawinputData = { 0 };
	UINT uiSize = sizeof(rawinputData);

	// 获取原始输入数据的大小
	::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawinputData, &uiSize, sizeof(RAWINPUTHEADER));
	if (RIM_TYPEKEYBOARD == rawinputData.header.dwType)
	{
		// WM_KEYDOWN --> 普通按键    WM_SYSKEYDOWN --> 系统按键(指的是ALT)  
		if ((WM_KEYDOWN == rawinputData.data.keyboard.Message) ||
			(WM_SYSKEYDOWN == rawinputData.data.keyboard.Message))
		{
			// 记录按键
			SaveKey(rawinputData.data.keyboard.VKey);
		}
	}
	return TRUE;
}

// 保存按键信息
void KeyBoardListener::SaveKey(USHORT usVKey) {
	TCHAR szKey[MAX_PATH] = { 0 };
	TCHAR szTitle[MAX_PATH] = { 0 };
	TCHAR szText[MAX_PATH] = { 0 };
	// 获取顶层窗口
	HWND hForegroundWnd = ::GetForegroundWindow();
	// 获取顶层窗口标题
	::GetWindowText(hForegroundWnd, szTitle, 256);
	// 将虚拟键码转换成对应的ASCII
	::lstrcpy(szKey, GetKeyName(usVKey));
	
	
	// 构造按键记录信息字符串
	::wsprintf(szText, TEXT("[%s] %s\r\n"), szTitle, szKey);
	
	// 添加数据
	ui.plainTextEdit->appendPlainText(QString::fromWCharArray(szText));
}

// (键盘)消息处理函数
bool KeyBoardListener::nativeEvent(const QByteArray& eventType, void* message, long* result) {
	MSG* msg = (MSG*)message;
	if (msg->message == WM_INPUT) {
		GetData(msg->lParam);
	}

	return false;
}
