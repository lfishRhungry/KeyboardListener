#pragma once

#include <QtWidgets/QWidget>
#include <QMessageBox>
#include "ui_KeyBoardListener.h"
#include <Windows.h>

class KeyBoardListener : public QWidget
{
	Q_OBJECT

public:
	KeyBoardListener(QWidget *parent = Q_NULLPTR);
	// �������
	void ShowError(PTCHAR pszText);
	// ע��ԭʼ�豸
	BOOL Register();
	// ��ȡԭʼ��������
	BOOL GetData(LPARAM lParam);
	// ���水����Ϣ
	void SaveKey(USHORT usVKey);
	// (����)��Ϣ������
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);

private:
	Ui::KeyBoardListenerClass ui;
	// �����򴰿ھ��
	HWND hWnd;
};
