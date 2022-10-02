/*
 * 文件名：mainWindow.h
 * 最后修改日期：2022.09.13
 * 作者：林政硕
 * 功能：主窗口头文件
*/

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMouseEvent>
#include <QTimer>

#include "titlebar.h"
#include "managementconnectclient.h"

#define PORTINIT 8888                       //默认端口号

#define NORMALMESRECTEXTEDITHIGHT 501       //正常情况下的recTextEdit的高
#define AFTERCONNECTMESRECTEXTEDITHIGHT 471 //udp模式下连接后的recTextEdit的高

/*网络模式*/
enum netMode{
    UDP = 0,
    TCP_Server,
    TCP_Client
};

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

/*主窗口类*/
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:

    /*****************************
    **@brief : 初始化主窗口样式
    **@param : none
    **@return: none
    *****************************/
    void initWindowStyle();

    /*****************************
    **@brief : 设置系统默认初始化
    **@param : none
    **@return: none
    *****************************/
    void initSysSetting();

    /*****************************
    **@brief : 初始化信号与槽连接
    **@param : none
    **@return: none
    *****************************/
    void initConnect();

    /*****************************
    **@brief : 加载样式表
    **@param : 样式表名称
    **@return: none
    *****************************/
    void loadStyleSheet(const QString &sheetName);

    /*****************************
    **@brief : 获取本地IP地址
    **@param : 存放ip的变量
    **@return: 返回true获取成功，flase则失败
    *****************************/
    bool getHostIp(QString &ip);

    /*****************************
    **@brief : 禁用当前网络设置
    **@param : false禁用
    **@param : true开启
    **@return: none
    *****************************/
    void forbidNetworkSet(bool state);

    /*****************************
    **@brief : 获取用户当前在网络模式下输入的ip
    **@param : 存放ip的变量
    **@return: none
    *****************************/
    void getCurrentIp(QString &ip);

    /*****************************
    **@brief : 获取用户当前在网络模式下输入的port
    **@param : 存放port的变量
    **@return: none
    *****************************/
    void getCurrentPort(QString &port);

    /*****************************
    **@brief : 获取用户当前在udp模式下输入的远程主机ip
    **@param : 存放ip的变量
    **@return: none
    *****************************/
    void getUdpRemoteIp(QString &ip);

    /*****************************
    **@brief : 获取用户当前在udp模式下输入的远程主机port
    **@param : 存放port的变量
    **@return: none
    *****************************/
    void getUdpRemotePort(QString &port);

    /*****************************
    **@brief : 设置udp模式下远程主机ip/port的可见性
    **@param : false禁用
    **@param : true开启
    **@return: none
    *****************************/
    void setUdpRemoteHostPortVisible(bool state);

    /*****************************
    **@brief : udp模式下数据发送
    **@param : 目标ip
    **@param : 目标端口
    **@return: none
    *****************************/
    void udpSendMes(QString remoteIp, int remotePort);

    /*****************************
    **@brief : tcp_server模式下数据发送
    **@param : index 循环序号
    **@return: none
    *****************************/
    void tcpServerSendMes(int index);

    /*****************************
    **@brief : tcp_client模式下数据发送
    **@param : index 循环序号
    **@return: none
    *****************************/
    void tcpClientSendMes();

    /*****************************
    **@brief : 禁用发送区设置
    **@param : false 禁用
    **@param : true 启用
    **@return: none
    *****************************/
    void forbidSendSetting(bool state);

private slots:

    /*****************************
    **@brief : 网络选择checkbox槽函数
    **@param : 下拉选项下标
    **@return: none
    *****************************/
    void onNetModeShow(int index);

    /*****************************
    **@brief : udp模式下数据读取
    **@param : none
    **@return: none
    *****************************/
    void onUDPReadMes();

    /*****************************
    **@brief : tcp_server模式下有新客户端连接槽函数
    **@param : none
    **@return: none
    *****************************/
    void onTCPNewConnect();

    /*****************************
    **@brief : tcp_server模式下断开连接槽函数（暂时没用到）
    **@param : none
    **@return: none
    *****************************/
    void onTCPServerDisconnect();

    /*****************************
    **@brief : tcp_server模式下数据读取
    **@param : none
    **@return: none
    *****************************/
    void onTCPServerReadMes();

    /*****************************
    **@brief : tcp_server模式下与客户端连接异常槽函数，
    **         用于检测客户端是否主动断开
    **@param : QAbstractSocket::SocketError
    **@return: none
    *****************************/
    void onMSGError(QAbstractSocket::SocketError);

    /*****************************
    **@brief : tcp_server下服务器主动断开与客户端的连接槽函数
    **@param : row 客户端管理窗口的目标客户端所处行号
    **@return: none
    *****************************/
    void onDisconnectToClients(QString row);

    /*****************************
    **@brief : tcp_client模式下数据读取
    **@param : none
    **@return: none
    *****************************/
    void onTCPClientReadMes();

    /*****************************
    **@brief : tcp_client下被动断开与服务器的连接槽函数
    **@param : none
    **@return: none
    *****************************/
    void onTCPClientBeDisconnect();

    /*****************************
    **@brief : timer定时器槽函数，用于循环发送数据
    **@param : none
    **@return: none
    *****************************/
    void onSendMesBtn();

    /*****************************
    **@brief : 连接按钮槽函数
    **@param : none
    **@return: none
    *****************************/
    void on_connectBtn_clicked();

    /*****************************
    **@brief : 发送数据按钮槽函数
    **@param : none
    **@return: none
    *****************************/
    void on_sendMesBtn_clicked();

    /*****************************
    **@brief : 清空发送区域按钮槽函数
    **@param : none
    **@return: none
    *****************************/
    void on_clearSendBtn_clicked();

    /*****************************
    **@brief : 清空接收区域按钮槽函数
    **@param : none
    **@return: none
    *****************************/
    void on_clearRecBtn_clicked();

    /*****************************
    **@brief : 复制ip按钮槽函数
    **@param : none
    **@return: none
    *****************************/
    void on_copyIPBtn_clicked();

    /*****************************
    **@brief : 获取当前填写ip按钮槽函数
    **@param : none
    **@return: none
    *****************************/
    void on_getLocalIpBtn_clicked();

    /*****************************
    **@brief : tcp_server模式下客户端管理按钮槽函数
    **@param : none
    **@return: none
    *****************************/
    void on_connectedClientBtn_clicked();

    /*****************************
    **@brief : 退出主窗口槽函数
    **@param : none
    **@return: none
    *****************************/
    void quitApplication();

    /*****************************
    **@brief : 保存文件槽函数
    **@param : arg1 选择状态
    **@return: none
    *****************************/
    void on_rec_saveToFileCheckBox_stateChanged(int arg1);

    /*****************************
    **@brief : 循环发送槽函数
    **@param : arg2 选择状态
    **@return: none
    *****************************/
    void on_send_dataCycleCheckBox_stateChanged(int arg1);

    void on_ipSetLineEdit1_textChanged(const QString &arg1);

    void on_ipSetLineEdit2_textChanged(const QString &arg1);

    void on_ipSetLineEdit3_textChanged(const QString &arg1);

    void on_udp_ipSetLineEdit1_textChanged(const QString &arg1);

    void on_udp_ipSetLineEdit2_textChanged(const QString &arg1);

    void on_udp_ipSetLineEdit3_textChanged(const QString &arg1);

private:

    Ui::Widget *ui;
    TitleBar *m_titleBar;                   //标题栏窗口
    ManagementConnectClient *manClients;    //客户端管理窗口

    QString currentIp;      //用于保存当前IP
    QString currentPort;    //用于保存当前端口号
    netMode currentNetMode; //用于保存当前网络模式
    bool connectState;      //连接状态

    QUdpSocket *udpUser;                    //udp模式
    QTcpSocket *tcpClientUser;              //tcp client模式

    QTcpServer *tcpServerUser;              //tcp server模式
    QTcpSocket *tempTcpSocketUser;          //用于存放临时客户端
    QList<QTcpSocket*> tcpSocketUserList;   //用于存放多个客户端
    int clientNum;                          //客户端数量

    QHostAddress targetAddr; //目标地址
    quint16 targetPort;      //目标端口

    QTimer *sendTimer;       //定时发送
    int sendTimerFlag;       //定时发送标志位 1开启 0未开启

};

#endif // WIDGET_H
