/*
 * 文件名：managementconnectclient.h
 * 最后修改日期：2022.09.13
 * 作者：林政硕
 * 功能：TCP_Server模式下的客户端管理窗口
*/

#ifndef MANAGEMENTCONNECTCLIENT_H
#define MANAGEMENTCONNECTCLIENT_H

#include "titlebar.h"

#include <QDialog>
#include <QTableWidget>
#include <QLabel>


namespace Ui {
class ManagementConnectClient;
}

class ManagementConnectClient : public QDialog
{
    Q_OBJECT

public:
    explicit ManagementConnectClient(QWidget *parent = nullptr);
    ~ManagementConnectClient();

    /*****************************
    **@brief : 在指定行中设置新内容
    **@param : x 指定行
    **@param : item 要显示的ip
    **@param : item2 要显示的port
    **@return: none
    *****************************/
    void setNewItem(unsigned int x, QTableWidgetItem *itemIP, QTableWidgetItem *itemPORT);

    /*****************************
    **@brief : 获取当前列表行数
    **@param : none
    **@return: none
    *****************************/
    int getRowCount();

    /*****************************
    **@brief : 获取当前行数据
    **@param : row 具体行数
    **@param : ip 存放ip的变量
    **@param : port 存放port的变量
    **@param : sendState 屏蔽发送标志位
    **@param : rec 屏蔽接收标志位
    **@return: none
    *****************************/
    void getRowData(int row, QString &ip, QString &port, QString &sendState, QString &recState);

    /*****************************
    **@brief : 插入新行
    **@param : none
    **@return: none
    *****************************/
    void insertNewRow();

    /*****************************
    **@brief : 获取当前行数据
    **@param : x 行号
    **@param : y 列号
    **@return: none
    *****************************/
    void setTextAli(int x, int y);

    /*****************************
    **@brief : 删除所有行
    **@param : none
    **@return: none
    *****************************/
    void deleteAllRow();

    /*****************************
    **@brief : 删除指定行
    **@param : index 行号
    **@return: none
    *****************************/
    void deleteOnceRow(int index);

    /*****************************
    **@brief : 设置客户端数量的显示文本
    **@param : count 已连接的客户端数量
    **@return: none
    *****************************/
    void setClientNumTabel(int count);

    /*****************************
    **@brief : 禁用断开连接序号输入框
    **@param : false 禁用
    **@param : true 启用
    **@return: none
    *****************************/
    void forbitDisconnectLineEditLineEdit(bool state);

private:

    /*****************************
    **@brief : 初始化窗口样式
    **@param : none
    **@return: none
    *****************************/
    void initDialogStyle();

    /*****************************
    **@brief : 初始化窗口系统设置
    **@param : none
    **@return: none
    *****************************/
    void initSysSetting();

    /*****************************
    **@brief : 加载样式表
    **@param : sheetName 样式表名称
    **@return: none
    *****************************/
    void loadStyleSheet(const QString &sheetName);  //加载样式表

signals:
    void disconnectToClient(QString row);

private slots:

    /*****************************
    **@brief : 断开连接按钮槽函数
    **@param : none
    **@return: none
    *****************************/
    void on_disconnectBtn_clicked();

private:
    Ui::ManagementConnectClient *ui;
    TitleBar *manClientsTitleBar;           //标题栏
    QTableWidget *manClientsTableWidget;    //客户端管理列表
    QWidget *m_parent;                      //保存主窗口指针

};

#endif // MANAGEMENTCONNECTCLIENT_H
