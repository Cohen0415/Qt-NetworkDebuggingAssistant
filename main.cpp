/*
 * 文件名：main.cpp
 * 最后修改日期：2022.10.2
 * 作者：林政硕
 * 功能：网络调试助手，用于工业、电子等需要网络通信
 *      调试的场合，目前有三种模式，分别是UDP, TCP服务器，
 *      TCP客户端。
 * 版本：v1.0
 * 开发环境：windows 10专业版 - Qt Creator 4.12.2
 * 备注：
*/

#include "mainWindow.h"
#include "titlebar.h"
#include "managementconnectclient.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}
