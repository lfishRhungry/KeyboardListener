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
	// 处理错误
	void ShowError(PTCHAR pszText);
	// 注册原始设备
	BOOL Register();
	// 获取原始输入数据
	BOOL GetData(LPARAM lParam);
	// 保存按键信息
	void SaveKey(USHORT usVKey);
	// (键盘)消息处理函数
	bool nativeEvent(const QByteArray& eventType, void* message, long* result);

private:
	Ui::KeyBoardListenerClass ui;
	// 本程序窗口句柄
	HWND hWnd;
};
