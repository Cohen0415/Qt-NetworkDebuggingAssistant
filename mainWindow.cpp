/*
 * 文件名：mainWindow.cpp
 * 最后修改日期：2022.09.13
 * 作者：林政硕
 * 功能：主窗口源文件
*/

#include "mainWindow.h"
#include "ui_mainWindow.h"

#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QComboBox>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    initWindowStyle();              //初始化窗口样式
    initSysSetting();               //初始化程序系统设置
    initConnect();                  //初始化信号与槽连接
    loadStyleSheet("mainWindow");   //加载样式表
}

Widget::~Widget()
{
    delete ui;
}

/*****************************
**@brief : 初始化主窗口样式
**@param : none
**@return: none
*****************************/
void Widget::initWindowStyle()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);	//透明
    this->setWindowTitle("网络调试助手");

    //在主窗口创建自定义标题栏
    m_titleBar = new TitleBar(this, mainWindows);
    m_titleBar->setTitleBarSize(width(), 50);
    m_titleBar->setTitleSlogan("网络调试助手");
    m_titleBar->move(0, 0);
}

/*****************************
**@brief : 设置系统默认初始化
**@param : none
**@return: none
*****************************/
void Widget::initSysSetting()
{

    //初始化客户端数量
    clientNum = 0;

    //三种模式初始化
    udpUser = new QUdpSocket(this);
    tcpServerUser = new QTcpServer(this);
    tcpClientUser = new QTcpSocket(this);

    //循环发送初始化
    QRegExp gapRegx("[1-9][0-9][0-9][0-9][0-9][0-9]");
    QValidator *gapValidator = new QRegExpValidator(gapRegx, ui->sendGapLineEdit);
    ui->sendGapLineEdit->setValidator(gapValidator);
    ui->sendGapLineEdit->setText("1000");
    sendTimerFlag = 0;

    //输入输出文本编辑框清空
    ui->mesRecTextEdit->clear();
    ui->sendMesTextEdit->clear();

    //协议类型初始化
    ui->netModeComboBox->setCurrentIndex(UDP);
    currentNetMode = UDP;

    //用户错误提示初始化
    ui->errorTipLabel->setText("");
    ui->errorTipLabel_2->setText("");

    //限制ip/port编辑器只能输入指定长度数字
    QRegExp ipRegx("[0-9][0-9][0-9]");
    QRegExp portRegx("[0-9][0-9][0-9][0-9][0-9]");
    QValidator *ipValidator = new QRegExpValidator(ipRegx, ui->ipSetLineEdit1);
    QValidator *portValidator = new QRegExpValidator(portRegx, ui->portLineEdit);
    ui->ipSetLineEdit1->setValidator(ipValidator);
    ui->ipSetLineEdit2->setValidator(ipValidator);
    ui->ipSetLineEdit3->setValidator(ipValidator);
    ui->ipSetLineEdit4->setValidator(ipValidator);
    ui->portLineEdit->setValidator(portValidator);

    ui->udp_ipSetLineEdit1->setValidator(ipValidator);
    ui->udp_ipSetLineEdit2->setValidator(ipValidator);
    ui->udp_ipSetLineEdit3->setValidator(ipValidator);
    ui->udp_ipSetLineEdit4->setValidator(ipValidator);
    ui->udp_portLineEdit->setValidator(portValidator);

    //udp模式下的远程主机和端口号填写默认在连接上后才显示
    setUdpRemoteHostPortVisible(false);

    //tcp server模式下的“管理已连接客户端”按钮初始化不显示
    ui->connectedClientBtn->setVisible(false);
    manClients = NULL;

    //获取本地IP地址显示到应用程序上
    QString ip;
    bool stateGetHostIp;
    stateGetHostIp = getHostIp(ip);
    //若成功获取ip
    if(stateGetHostIp){
        //再次判断
        if(!ip.isEmpty()){
            QStringList afterSplitIp = ip.split(".");
            ui->ipSetLineEdit1->setText(afterSplitIp.at(0));
            ui->ipSetLineEdit2->setText(afterSplitIp.at(1));
            ui->ipSetLineEdit3->setText(afterSplitIp.at(2));
            ui->ipSetLineEdit4->setText(afterSplitIp.at(3));
            currentIp = ip;
        }
    }
    else{}

    //默认端口号初始化
    ui->portLineEdit->setText(QString::number(PORTINIT));
    currentPort = QString::number(PORTINIT);

    //发送接收区初始化
    ui->send_autoClearCheckBox->setChecked(false);
    ui->send_dataCycleCheckBox->setChecked(false);
    ui->rec_pauseRecCheckBox->setChecked(false);
    ui->rec_autoReturnCheckBox->setChecked(false);

    //接收区文本编辑器的高度初始化
    ui->mesRecTextEdit->setFixedHeight(NORMALMESRECTEXTEDITHIGHT);

    //发送按钮初始化
    ui->connectBtn->setText("打 开");
}

/*****************************
**@brief : 初始化信号与槽连接
**@param : none
**@return: none
*****************************/
void Widget::initConnect()
{
    //下拉框
    connect(ui->netModeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onNetModeShow(int)));
    connect(m_titleBar, SIGNAL(quitMainWindow()),
            this, SLOT(quitApplication()));
    connect(tcpServerUser, &QTcpServer::newConnection,
            this, &Widget::onTCPNewConnect);
    connect(tcpClientUser, &QTcpSocket::readyRead,
            this, &Widget::onTCPClientReadMes);
    connect(tcpClientUser, &QAbstractSocket::disconnected,
            this, &Widget::onTCPClientBeDisconnect);
}

/*****************************
**@brief : 加载样式表
**@param : 样式表名称
**@return: none
*****************************/
void Widget::loadStyleSheet(const QString &sheetName)
{
    QFile file(":/Resources/qss/" + sheetName + ".css");
    file.open(QFile::ReadOnly);
    if(file.isOpen()){
        QString styleSheet = this->styleSheet();
        styleSheet += QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
    }
    file.close();
}

/*****************************
**@brief : 获取本地IP地址
**@param : 存放ip的变量
**@return: 返回true获取成功，flase则失败
*****************************/
bool Widget::getHostIp(QString &ip)
{
    QString ipAdress;
    QList<QHostAddress> ipAdressList = QNetworkInterface::allAddresses();

    if(ipAdressList.isEmpty())
        return false;

    for(int i = 0; i < ipAdressList.size(); ++i){
        QHostAddress ipAddr = ipAdressList.at(i);
        //筛选ipv4地址
        if(ipAddr.protocol() == QAbstractSocket::IPv4Protocol){
            ip = ipAddr.toString();
            return true;
        }
    }

    return true;

}

/*****************************
**@brief : 禁用当前网络设置
**@param : false禁用
**@param : true开启
**@return: none
*****************************/
void Widget::forbidNetworkSet(bool state)
{
    ui->netModeComboBox->setEnabled(state);
    ui->ipSetLineEdit1->setEnabled(state);
    ui->ipSetLineEdit2->setEnabled(state);
    ui->ipSetLineEdit3->setEnabled(state);
    ui->ipSetLineEdit4->setEnabled(state);
    ui->portLineEdit->setEnabled(state);
}

/*****************************
**@brief : 获取用户当前在网络模式下输入的ip
**@param : 存放ip的变量
**@return: none
*****************************/
void Widget::getCurrentIp(QString &ip)
{
    QString tempIp = ui->ipSetLineEdit1->text() + "." + ui->ipSetLineEdit2->text() + "." +
                     ui->ipSetLineEdit3->text() + "." + ui->ipSetLineEdit4->text();
    ip = tempIp;
    qDebug() << "In getCurrentIp(): " << ip;
}

/*****************************
**@brief : 获取用户当前在网络模式下输入的port
**@param : 存放port的变量
**@return: none
*****************************/
void Widget::getCurrentPort(QString &port)
{
    QString tempPort = ui->portLineEdit->text();
    port = tempPort;
    qDebug() << "In getCurrentPort(): " << port;
}

/*****************************
**@brief : 获取用户当前在udp模式下输入的远程主机ip
**@param : 存放ip的变量
**@return: none
*****************************/
void Widget::getUdpRemoteIp(QString &ip)
{
    QString tempIp = ui->udp_ipSetLineEdit1->text() + "." + ui->udp_ipSetLineEdit2->text() + "." +
                     ui->udp_ipSetLineEdit3->text() + "." + ui->udp_ipSetLineEdit4->text();
    ip = tempIp;
    qDebug() << "In getUdpRemoteIp(): " << ip;
}

/*****************************
**@brief : 获取用户当前在udp模式下输入的远程主机port
**@param : 存放port的变量
**@return: none
*****************************/
void Widget::getUdpRemotePort(QString &port)
{
    QString tempPort = ui->udp_portLineEdit->text();
    port = tempPort;
    qDebug() << "In getUdpRemotePort(): " << port;
}

/*****************************
**@brief : 设置udp模式下远程主机ip/port的可见性
**@param : false禁用
**@param : true开启
**@return: none
*****************************/
void Widget::setUdpRemoteHostPortVisible(bool state)
{
    ui->udp_remoteHostLabel->setVisible(state);
    ui->udp_remotePortLabel->setVisible(state);
    ui->udp_ipSetLineEdit1->setVisible(state);
    ui->udp_ipSetLineEdit2->setVisible(state);
    ui->udp_ipSetLineEdit3->setVisible(state);
    ui->udp_ipSetLineEdit4->setVisible(state);
    ui->udp_pointLabel1->setVisible(state);
    ui->udp_pointLabel2->setVisible(state);
    ui->udp_pointLabel3->setVisible(state);
    ui->udp_portLineEdit->setVisible(state);
}

/*****************************
**@brief : udp模式下数据发送
**@param : 目标ip
**@param : 目标端口
**@return: none
*****************************/
void Widget::udpSendMes(QString remoteIp, int remotePort)
{
    QByteArray dataGram = ui->sendMesTextEdit->document()->toRawText().toLocal8Bit();
    udpUser->writeDatagram(dataGram.data(), dataGram.size(),
                           QHostAddress(remoteIp), remotePort);
}

void Widget::tcpServerSendMes(int index)
{
    QByteArray dataGram = ui->sendMesTextEdit->document()->toRawText().toLocal8Bit();
    tcpSocketUserList.at(index)->write(dataGram.data());
    tcpSocketUserList.at(index)->flush();
}

void Widget::tcpClientSendMes()
{
    QByteArray dataGram = ui->sendMesTextEdit->document()->toRawText().toLocal8Bit();
    tcpClientUser->flush();
    tcpClientUser->write(dataGram.data());
}

/*****************************
**@brief : 禁用发送区设置
**@param : false 禁用
**@param : true 启用
**@return: none
*****************************/
void Widget::forbidSendSetting(bool state)
{
    ui->clearSendBtn->setEnabled(state);
    ui->send_autoClearCheckBox->setEnabled(state);
    ui->send_dataCycleCheckBox->setEnabled(state);
    ui->sendGapLineEdit->setEnabled(state);
    ui->sendMesTextEdit->setEnabled(state);
}

/*****************************
**@brief : 网络选择checkbox槽函数
**@param : 下拉选项下标
**@return: none
*****************************/
void Widget::onNetModeShow(int index)
{
    if(index == UDP){
        ui->netSetSecondLabel->setText("(2)本地IP地址");
        ui->netSetThirdLabel->setText("(3)本地端口号");
        ui->connectBtn->setText("打 开");
        currentNetMode = UDP;
        qDebug() << "当前模式：UDP";
    }
    else if(index == TCP_Server){
        ui->netSetSecondLabel->setText("(2)本地IP地址");
        ui->netSetThirdLabel->setText("(3)本地端口号");
        ui->connectBtn->setText("开始监听");
        currentNetMode = TCP_Server;
        qDebug() << "当前模式：TCP_Server";
    }
    else if(index == TCP_Client){
        ui->netSetSecondLabel->setText("(2)服务器IP地址");
        ui->netSetThirdLabel->setText("(3)服务器端口号");
        ui->connectBtn->setText("连 接");
        currentNetMode = TCP_Client;
        qDebug() << "当前模式：TCP_Client";
    }
}

/*****************************
**@brief : udp模式下数据读取
**@param : none
**@return: none
*****************************/
void Widget::onUDPReadMes()
{
    //有未处理的数据报
    while(udpUser->hasPendingDatagrams()){

        QDateTime current_date_time = QDateTime::currentDateTime();
        QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");

        QByteArray dataGram;
        dataGram.resize(udpUser->pendingDatagramSize());

        udpUser->readDatagram(dataGram.data(), dataGram.size(), &targetAddr, &targetPort);
        QTextCodec *after = QTextCodec::codecForName("GBK");
        QString tmp = after->toUnicode(dataGram);

        if(ui->rec_pauseRecCheckBox->checkState()){
            return;
        }

        if(ui->rec_autoReturnCheckBox->checkState()){
            ui->mesRecTextEdit->append("");
        }

        QString terminalText;
        terminalText = "[" + current_date + " Receive From " + targetAddr.toString() + ":" +
                    QString::number(targetPort) + "]<<<:";
        ui->mesRecTextEdit->setTextColor("blue");
        ui->mesRecTextEdit->append(terminalText);
        terminalText = tmp;
        ui->mesRecTextEdit->setTextColor("black");
        ui->mesRecTextEdit->append(terminalText);
    }

}

/*****************************
**@brief : tcp_server模式下有新客户端连接槽函数
**@param : none
**@return: none
*****************************/
void Widget::onTCPNewConnect()
{
    //当前连接来的客户端
    tempTcpSocketUser = tcpServerUser->nextPendingConnection();
    tcpSocketUserList.append(tempTcpSocketUser);

    //断开连接
    connect(tempTcpSocketUser, &QTcpSocket::disconnected,
            tempTcpSocketUser, &QTcpSocket::deleteLater);
    connect(tempTcpSocketUser, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onMSGError(QAbstractSocket::SocketError)));
    connect(tempTcpSocketUser, &QTcpSocket::disconnected,
            this, &Widget::onTCPServerDisconnect);

    connect(tempTcpSocketUser, &QTcpSocket::readyRead,
            this, &Widget::onTCPServerReadMes);

    QString clientIP = tcpSocketUserList.at(clientNum)->peerAddress().toString();
    QString clientPort = QString::number(tcpSocketUserList.at(clientNum)->peerPort());

    manClients->insertNewRow();
    QString rowCount = QString::number(manClients->getRowCount());
    qDebug() << "manClients当前行数：" << rowCount;
    manClients->setNewItem(rowCount.toInt()-1,
                           new QTableWidgetItem(clientIP),
                           new QTableWidgetItem(clientPort));

    clientNum++;
    manClients->setClientNumTabel(clientNum);
}

/*****************************
**@brief : tcp_server模式下断开连接槽函数（暂时没用到）
**@param : none
**@return: none
*****************************/
void Widget::onTCPServerDisconnect()
{}

/*****************************
**@brief : tcp_server模式下数据读取
**@param : none
**@return: none
*****************************/
void Widget::onTCPServerReadMes()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");

    QByteArray dataGram;
    QString rowIp, rowPort, sendState, recState;

    for(int i=0; i<tcpSocketUserList.size(); i++){
        manClients->getRowData(i, rowIp, rowPort, sendState, recState);

        tempTcpSocketUser = tcpSocketUserList.at(i);
        dataGram = tempTcpSocketUser->readAll();
        QTextCodec *after = QTextCodec::codecForName("GBK");
        QString tmp = after->toUnicode(dataGram);

        if(recState == "否"){
            if(dataGram.length() != 0){ //数据不等于0，则说明是这个客户端发的数据

                if(ui->rec_pauseRecCheckBox->checkState()){
                    return;
                }

                if(ui->rec_autoReturnCheckBox->checkState()){
                    ui->mesRecTextEdit->append("");
                }

                QString clientIP = tempTcpSocketUser->peerAddress().toString();
                QString clientPort = QString::number(tempTcpSocketUser->peerPort());

                QString terminalText;
                terminalText = "[" + current_date + " Receive From " + clientIP + ":" +
                            clientPort + "]<<<:";
                ui->mesRecTextEdit->setTextColor("blue");
                ui->mesRecTextEdit->append(terminalText);
                terminalText = tmp;
                ui->mesRecTextEdit->setTextColor("black");
                ui->mesRecTextEdit->append(terminalText);
            }
        }
    }

}

/*****************************
**@brief : tcp_server模式下与客户端连接异常槽函数，
**         用于检测客户端是否主动断开
**@param : QAbstractSocket::SocketError
**@return: none
*****************************/
void Widget::onMSGError(QAbstractSocket::SocketError)
{
    for(int i=0; i<tcpSocketUserList.size(); i++)
    {
        int error = tcpSocketUserList.at(i)->error();
        if(error == QAbstractSocket::RemoteHostClosedError)
        {
            QString tempHostIP = tcpSocketUserList.at(i)->peerAddress().toString();
            int tempHostPort = tcpSocketUserList.at(i)->QAbstractSocket::peerPort();
            QString rowIp, rowPort, sendState, recState;

            //获取table中的数据
            manClients->getRowData(i, rowIp, rowPort, sendState, recState);
            qDebug() << "sendState: " << sendState;
            qDebug() << "recState: " << recState;
            //与list中的ip port比较，相同则删除（目的主要是检测删除目标是否正确）
            if(rowIp == tempHostIP && rowPort == QString::number(tempHostPort)){
                tcpSocketUserList.removeAt(i);
                manClients->deleteOnceRow(i);
            }
            else{
                //显示错误代码，直接退出程序
            }

            clientNum--;
            manClients->setClientNumTabel(clientNum);
            if(clientNum == 0 && sendTimerFlag == 1){
                on_sendMesBtn_clicked();
            }
        }
    }
}

/*****************************
**@brief : tcp_server下服务器主动断开与客户端的连接槽函数
**@param : row 客户端管理窗口的目标客户端所处行号
**@return: none
*****************************/
void Widget::onDisconnectToClients(QString row)
{
    qDebug() << "in onDisconnectToClients";
    int tempRow = row.toInt();
    if(tempRow >= 1 && tempRow <= clientNum){
        qDebug() << "in onDisconnectToClients_2";
        tcpSocketUserList.at(tempRow-1)->close();
        tcpSocketUserList.removeAt(tempRow-1);
        manClients->deleteOnceRow(tempRow-1);
        clientNum--;
        manClients->setClientNumTabel(clientNum);
    }
}

/*****************************
**@brief : tcp_client模式下数据读取
**@param : none
**@return: none
*****************************/
void Widget::onTCPClientReadMes()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss");

    QString serverIP = tcpClientUser->peerAddress().toString();
    QString serverPort = QString::number(tcpClientUser->peerPort());

    QByteArray bt;
    bt.resize(tcpClientUser->bytesAvailable());
    tcpClientUser->read(bt.data(), bt.size());

    QTextCodec *after = QTextCodec::codecForName("GBK");
    QString tmp = after->toUnicode(bt);

    if(ui->rec_pauseRecCheckBox->checkState()){
        return;
    }

    if(ui->rec_autoReturnCheckBox->checkState()){
        ui->mesRecTextEdit->append("");
    }

    QString terminalText;
    terminalText = "[" + current_date + " Receive From " + serverIP + ":" +
            serverPort + "]<<<:";
    ui->mesRecTextEdit->setTextColor("blue");
    ui->mesRecTextEdit->append(terminalText);
    terminalText = tmp;
    ui->mesRecTextEdit->setTextColor("black");
    ui->mesRecTextEdit->append(terminalText);
}

/*****************************
**@brief : tcp_client下被动断开与服务器的连接槽函数
**@param : none
**@return: none
*****************************/
void Widget::onTCPClientBeDisconnect()
{
    qDebug() << "onTCPClientBeDisconnect";
    on_connectBtn_clicked();
}

/*****************************
**@brief : timer定时器槽函数，用于循环发送数据
**@param : none
**@return: none
*****************************/
void Widget::onSendMesBtn()
{
    if(currentNetMode == UDP){

        if(connectState){
            QString remoteIP;
            QString remotePort;
            getUdpRemoteIp(remoteIP);
            getUdpRemotePort(remotePort);
            udpSendMes(remoteIP, remotePort.toInt());
        }

    }
    else if(currentNetMode == TCP_Server){

        QString rowIp, rowPort, sendState, recState;
        for(int i = 0; i < tcpSocketUserList.size(); i++){
            manClients->getRowData(i, rowIp, rowPort, sendState, recState);
            if(sendState == "否"){
                tcpServerSendMes(i);
            }
        }

    }
    else if(currentNetMode == TCP_Client){
        tcpClientSendMes();
    }
}

/*****************************
**@brief : 连接按钮槽函数
**@param : none
**@return: none
*****************************/
void Widget::on_connectBtn_clicked()
{
    //判断无效IP和端口
    if(ui->ipSetLineEdit1->text().isEmpty() || ui->ipSetLineEdit2->text().isEmpty() || ui->ipSetLineEdit3->text().isEmpty() || ui->ipSetLineEdit4->text().isEmpty()){
        ui->errorTipLabel->setText("无效 I P");
        return;
    }
    else if(ui->portLineEdit->text().isEmpty()){
        ui->errorTipLabel->setText("无效端口");
        return;
    }
    else{
        ui->errorTipLabel->setText("");
    }


    //如果当前未连接，则去连接
    if(!connectState){

        //当前模式为UDP
        if(currentNetMode == UDP){

            getCurrentIp(currentIp);
            getCurrentPort(currentPort);
            connectState = udpUser->bind(QHostAddress(currentIp), currentPort.toInt(), QUdpSocket::ShareAddress);
            qDebug() << "connectState: " << connectState;

            if(!connectState)
                return;

            ui->udp_ipSetLineEdit1->setText(ui->ipSetLineEdit1->text());
            ui->udp_ipSetLineEdit2->setText(ui->ipSetLineEdit2->text());
            ui->udp_ipSetLineEdit3->setText(ui->ipSetLineEdit3->text());
            ui->udp_ipSetLineEdit4->setText(ui->ipSetLineEdit4->text());
            ui->udp_portLineEdit->setText(ui->portLineEdit->text());

            ui->mesRecTextEdit->setFixedHeight(AFTERCONNECTMESRECTEXTEDITHIGHT);
            setUdpRemoteHostPortVisible(true);
            ui->connectBtn->setText("断 开");
            forbidNetworkSet(false);
            ui->getLocalIpBtn->setVisible(false);

            connect(udpUser, &QUdpSocket::readyRead, this, &Widget::onUDPReadMes);

        }
        //当前模式为TCP_Server
        else if(currentNetMode == TCP_Server){

            manClients = new ManagementConnectClient();
            manClients->setClientNumTabel(clientNum);
            connect(manClients, SIGNAL(disconnectToClient(QString)),
                    this, SLOT(onDisconnectToClients(QString)));

            getCurrentIp(currentIp);
            getCurrentPort(currentPort);
            connectState = tcpServerUser->listen(QHostAddress(currentIp), currentPort.toInt());

            if(!connectState){
                qDebug() << tcpServerUser->errorString();
                return;
            }

            ui->connectedClientBtn->setVisible(true);
            ui->connectBtn->setText("断 开");
            forbidNetworkSet(false);
            ui->getLocalIpBtn->setVisible(false);


        }
        //当前模式为TCP_Client
        else if(currentNetMode == TCP_Client){
            getCurrentIp(currentIp);
            getCurrentPort(currentPort);
            tcpClientUser->abort();
            tcpClientUser->connectToHost(currentIp,currentPort.toInt());
            if(tcpClientUser->waitForConnected(500)){
                connectState = true;
            }
            else{
                connectState = false;
                qDebug() << "client err: " << tcpClientUser->error();
                return;
            }

            ui->connectBtn->setText("断 开");
            forbidNetworkSet(false);
            ui->getLocalIpBtn->setVisible(false);

        }

    }
    //如果当前是连接，则断开连接
    else{

        if(currentNetMode == UDP){
            //循环模式下直接断开连接
            if(sendTimerFlag == 1){
                on_sendMesBtn_clicked();
            }

            udpUser->abort();

            forbidNetworkSet(true);
            ui->connectBtn->setText("打 开");
            ui->getLocalIpBtn->setVisible(true);
            ui->mesRecTextEdit->setFixedHeight(NORMALMESRECTEXTEDITHIGHT);
            setUdpRemoteHostPortVisible(false);
            connectState = false;

        }
        else if(currentNetMode == TCP_Server){
            //循环模式下直接断开连接
            if(sendTimerFlag == 1){
                on_sendMesBtn_clicked();
            }

            if(tcpSocketUserList.size() > 0){
                for(int i = 0; i < tcpSocketUserList.size(); i++){
                    tcpSocketUserList.at(i)->close();
                    tcpSocketUserList.removeAt(i);
                    manClients->deleteOnceRow(i);
                }
            }

            tcpServerUser->close();
            delete manClients;
            manClients = NULL;
            forbidNetworkSet(true);
            ui->connectedClientBtn->setVisible(false);
            ui->getLocalIpBtn->setVisible(true);
            ui->connectBtn->setText("开始监听");
            connectState = false;
            clientNum = 0;
        }
        else if(currentNetMode == TCP_Client){
            if(tcpClientUser != NULL){
                qDebug() << "tcpClientUser->disconnectFromHost";
                if(sendTimerFlag == 1){
                    on_sendMesBtn_clicked();
                }
                tcpClientUser->disconnectFromHost();

                forbidNetworkSet(true);
                ui->getLocalIpBtn->setVisible(true);
                ui->connectBtn->setText("连 接");

                connectState = false;
            }
        }
    }

}

/*****************************
**@brief : 发送数据按钮槽函数
**@param : none
**@return: none
*****************************/
void Widget::on_sendMesBtn_clicked()
{
    if(sendTimerFlag == 1){
        sendTimer->stop();
        ui->sendMesBtn->setText("发\n送");
        forbidSendSetting(true);
        sendTimerFlag = 0;

        if(currentNetMode == UDP){
            ui->udp_ipSetLineEdit1->setEnabled(true);
            ui->udp_ipSetLineEdit2->setEnabled(true);
            ui->udp_ipSetLineEdit3->setEnabled(true);
            ui->udp_ipSetLineEdit4->setEnabled(true);
            ui->udp_portLineEdit->setEnabled(true);
        }
        if(currentNetMode == TCP_Server){
            manClients->forbitDisconnectLineEditLineEdit(true);
        }
        return;
    }

    if(!connectState){
        ui->errorTipLabel_2->setText("当前无连接对象");
        return;
    }

    if(ui->sendMesTextEdit->document()->isEmpty()){
        ui->errorTipLabel_2->setText("发送内容不允许为空");
        return;
    }
    ui->errorTipLabel_2->setText("");

    if(currentNetMode == TCP_Server && clientNum <= 0){
        ui->errorTipLabel_2->setText("当前没有客户端连接");
        return;
    }

    if(ui->send_dataCycleCheckBox->checkState()){
        if(ui->send_autoClearCheckBox->checkState()){
            QMessageBox::warning(this, tr("提示"), tr("循环发送下不能同时勾选自动清空"));
            return;
        }
        if(ui->sendGapLineEdit->text() == ""){
            QMessageBox::warning(this, tr("提示"), tr("循环间隔时间不能为空"));
            return;
        }
        int gapTime = ui->sendGapLineEdit->text().toInt();
        if(gapTime < 100){
            QMessageBox::warning(this, tr("提示"), tr("循环间隔时间不能小于100ms"));
            return;
        }
        sendTimer->start(gapTime);
        ui->sendMesBtn->setText("停\n止\n发\n送");
        forbidSendSetting(false);
        if(currentNetMode == TCP_Server){
            manClients->forbitDisconnectLineEditLineEdit(false);
        }
        sendTimerFlag = 1;
    }

    if(currentNetMode == UDP){
        //判断远程无效IP和端口
        if(ui->udp_ipSetLineEdit1->text().isEmpty() || ui->udp_ipSetLineEdit2->text().isEmpty() || ui->udp_ipSetLineEdit3->text().isEmpty() || ui->udp_ipSetLineEdit4->text().isEmpty()){
            ui->errorTipLabel_2->setText("无效的远程I P");
            return;
        }
        else if(ui->udp_portLineEdit->text().isEmpty()){
            ui->errorTipLabel_2->setText("无效的远程端口");
            return;
        }
        else{
            ui->errorTipLabel_2->setText("");
        }

        if(sendTimerFlag == 1){
            ui->udp_ipSetLineEdit1->setEnabled(false);
            ui->udp_ipSetLineEdit2->setEnabled(false);
            ui->udp_ipSetLineEdit3->setEnabled(false);
            ui->udp_ipSetLineEdit4->setEnabled(false);
            ui->udp_portLineEdit->setEnabled(false);
        }

        if(connectState){
            QString remoteIP;
            QString remotePort;
            getUdpRemoteIp(remoteIP);
            getUdpRemotePort(remotePort);
            udpSendMes(remoteIP, remotePort.toInt());
        }

    }
    else if(currentNetMode == TCP_Server){

        QString rowIp, rowPort, sendState, recState;
        for(int i = 0; i < tcpSocketUserList.size(); i++){
            manClients->getRowData(i, rowIp, rowPort, sendState, recState);
            if(sendState == "否"){
                tcpServerSendMes(i);
            }
        }
    }
    else if(currentNetMode == TCP_Client){
        tcpClientSendMes();
    }

    if(ui->send_autoClearCheckBox->checkState()){
        ui->sendMesTextEdit->clear();
    }
}

/*****************************
**@brief : 清空发送区域按钮槽函数
**@param : none
**@return: none
*****************************/
void Widget::on_clearSendBtn_clicked()
{
    ui->sendMesTextEdit->clear();
}

/*****************************
**@brief : 清空接收区域按钮槽函数
**@param : none
**@return: none
*****************************/
void Widget::on_clearRecBtn_clicked()
{
    ui->mesRecTextEdit->clear();
}

/*****************************
**@brief : 复制ip按钮槽函数
**@param : none
**@return: none
*****************************/
void Widget::on_copyIPBtn_clicked()
{
    QClipboard *board = QApplication::clipboard();
    QString tempIp;
    getCurrentIp(tempIp);
    board->setText(tempIp);
}

/*****************************
**@brief : 获取当前填写ip按钮槽函数
**@param : none
**@return: none
*****************************/
void Widget::on_getLocalIpBtn_clicked()
{
    QString ip;
    bool stateGetHostIp;
    stateGetHostIp = getHostIp(ip);
    //若成功获取ip
    if(stateGetHostIp){
        //再次判断
        if(!ip.isEmpty()){
            QStringList afterSplitIp = ip.split(".");
            ui->ipSetLineEdit1->setText(afterSplitIp.at(0));
            ui->ipSetLineEdit2->setText(afterSplitIp.at(1));
            ui->ipSetLineEdit3->setText(afterSplitIp.at(2));
            ui->ipSetLineEdit4->setText(afterSplitIp.at(3));
        }
    }
}

/*****************************
**@brief : tcp_server模式下客户端管理按钮槽函数
**@param : none
**@return: none
*****************************/
void Widget::on_connectedClientBtn_clicked()
{

    if(manClients->isHidden()){
        manClients->show();
        manClients->activateWindow();
    }else{
        manClients->activateWindow();
    }

}

/*****************************
**@brief : 退出主窗口槽函数
**@param : none
**@return: none
*****************************/
void Widget::quitApplication()
{
    QApplication::quit();
}

/*****************************
**@brief : 保存文件槽函数
**@param : arg1 选择状态
**@return: none
*****************************/
void Widget::on_rec_saveToFileCheckBox_stateChanged(int arg1)
{

    //如果选择则保存至文件
    if(arg1 == Qt::Checked){

        QFileDialog dlg(this);
        QString fileName = dlg.getSaveFileName(this, tr("另存为"), "新文本文件",
                                               tr("Text File(*.txt)"));
        if(fileName == "")
            return;

        QFile file(fileName);

        if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&file);

            out << ui->mesRecTextEdit->toPlainText() << endl;
            QMessageBox::warning(this, tr("提示"), tr("保存成功!"));
            file.close();
        }

    }

    ui->rec_saveToFileCheckBox->setCheckState(Qt::Unchecked);

}

/*****************************
**@brief : 循环发送槽函数
**@param : arg2 选择状态
**@return: none
*****************************/
void Widget::on_send_dataCycleCheckBox_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        sendTimer = new QTimer(this);
        connect(sendTimer, SIGNAL(timeout()),
                this, SLOT(onSendMesBtn()));
    }
    else{
        if(sendTimer != NULL){
            delete sendTimer;
            sendTimer = NULL;
        }
    }
}

void Widget::on_ipSetLineEdit1_textChanged(const QString &arg1)
{
    if(arg1.length() == 3){
        ui->ipSetLineEdit2->setFocus();
        ui->ipSetLineEdit2->selectAll();
    }
}

void Widget::on_ipSetLineEdit2_textChanged(const QString &arg1)
{
    if(arg1.length() == 3){
        ui->ipSetLineEdit3->setFocus();
        ui->ipSetLineEdit3->selectAll();
    }
}

void Widget::on_ipSetLineEdit3_textChanged(const QString &arg1)
{
    if(arg1.length() == 3){
        ui->ipSetLineEdit4->setFocus();
        ui->ipSetLineEdit4->selectAll();
    }
}

void Widget::on_udp_ipSetLineEdit1_textChanged(const QString &arg1)
{
    if(arg1.length() == 3){
        ui->udp_ipSetLineEdit2->setFocus();
        ui->udp_ipSetLineEdit2->selectAll();
    }
}



void Widget::on_udp_ipSetLineEdit2_textChanged(const QString &arg1)
{
    if(arg1.length() == 3){
        ui->udp_ipSetLineEdit3->setFocus();
        ui->udp_ipSetLineEdit3->selectAll();
    }
}

void Widget::on_udp_ipSetLineEdit3_textChanged(const QString &arg1)
{
    if(arg1.length() == 3){
        ui->udp_ipSetLineEdit4->setFocus();
        ui->udp_ipSetLineEdit4->selectAll();
    }
}
