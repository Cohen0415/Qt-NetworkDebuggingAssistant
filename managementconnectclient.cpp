/*
 * 文件名：managementconnectclient.cpp
 * 最后修改日期：2022.09.13
 * 作者：林政硕
 * 功能：TCP_Server模式下的客户端管理窗口
*/

#include "managementconnectclient.h"
#include "ui_managementconnectclient.h"

#include <QFile>
#include <QPushButton>
#include <QComboBox>
#include <QDebug>

ManagementConnectClient::ManagementConnectClient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ManagementConnectClient)
{
    m_parent = parent;
    ui->setupUi(this);
    initDialogStyle();
    initSysSetting();
    loadStyleSheet("manClients");
}

ManagementConnectClient::~ManagementConnectClient()
{
    delete ui;
}

/*****************************
**@brief : 在指定行中设置新内容
**@param : x 指定行
**@param : item 要显示的ip
**@param : item2 要显示的port
**@return: none
*****************************/
void ManagementConnectClient::setNewItem(unsigned int x, QTableWidgetItem *itemIP, QTableWidgetItem *itemPORT)
{
    manClientsTableWidget->setItem(x, 0, itemIP);
    manClientsTableWidget->setItem(x, 1, itemPORT);
    manClientsTableWidget->item(x, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    manClientsTableWidget->item(x, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QComboBox *cbb1 = new QComboBox();
    cbb1->setObjectName("sendComboBox");
    cbb1->setFocusPolicy(Qt::NoFocus);
    cbb1->addItems({"是", "否"});
    cbb1->setCurrentIndex(1);
    QComboBox *cbb2 = new QComboBox();
    cbb1->setObjectName("recComboBox");
    cbb2->setFocusPolicy(Qt::NoFocus);
    cbb2->addItems({"是", "否"});
    cbb2->setCurrentIndex(1);

    manClientsTableWidget->setCellWidget(x, 2, cbb1);
    manClientsTableWidget->setCellWidget(x, 3, cbb2);

}

/*****************************
**@brief : 获取当前列表行数
**@param : none
**@return: none
*****************************/
int ManagementConnectClient::getRowCount()
{
    return manClientsTableWidget->rowCount();
}

/*****************************
**@brief : 获取当前行数据
**@param : row 具体行数
**@param : ip 存放ip的变量
**@param : port 存放port的变量
**@param : sendState 屏蔽发送标志位
**@param : rec 屏蔽接收标志位
**@return: none
*****************************/
void ManagementConnectClient::getRowData(int row, QString &ip, QString &port,
                                         QString &sendState, QString &recState)
{
    ip = manClientsTableWidget->item(row, 0)->text();
    port = manClientsTableWidget->item(row, 1)->text();
    sendState = dynamic_cast<QComboBox *>(manClientsTableWidget->cellWidget(row,2))->currentText();
    recState = dynamic_cast<QComboBox *>(manClientsTableWidget->cellWidget(row,3))->currentText();
}

/*****************************
**@brief : 插入新行
**@param : none
**@return: none
*****************************/
void ManagementConnectClient::insertNewRow()
{
    manClientsTableWidget->insertRow(getRowCount());
}

/*****************************
**@brief : 获取当前行数据
**@param : x 行号
**@param : y 列号
**@return: none
*****************************/
void ManagementConnectClient::setTextAli(int x, int y)
{
    manClientsTableWidget->item(x, y)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

/*****************************
**@brief : 删除所有行
**@param : none
**@return: none
*****************************/
void ManagementConnectClient::deleteAllRow()
{
    int rowCount = getRowCount();
    while(rowCount){
        manClientsTableWidget->removeRow(rowCount-1);
        rowCount--;
    }
}

/*****************************
**@brief : 删除指定行
**@param : index 行号
**@return: none
*****************************/
void ManagementConnectClient::deleteOnceRow(int index)
{
    manClientsTableWidget->removeRow(index);
}

/*****************************
**@brief : 设置客户端数量的显示文本
**@param : count 已连接的客户端数量
**@return: none
*****************************/
void ManagementConnectClient::setClientNumTabel(int count)
{
    QString tipLabel = "当前连接数：" + QString::number(count);
    ui->clientsCountLabel->setText(tipLabel);
}

void ManagementConnectClient::forbitDisconnectLineEditLineEdit(bool state)
{
    ui->disconnectLineEdit->setEnabled(state);
    ui->disconnectBtn->setEnabled(state);
}

/*****************************
**@brief : 初始化窗口样式
**@param : none
**@return: none
*****************************/
void ManagementConnectClient::initDialogStyle()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);	//透明
    this->setWindowTitle("TCP Server - 客户端管理");

    manClientsTitleBar = new TitleBar(this, manClientsWindows);
    manClientsTitleBar->setTitleBarSize(width(), 50);
    manClientsTitleBar->setTitleSlogan("TCP Server - 客户端管理");
    manClientsTitleBar->move(0, 0);
}

/*****************************
**@brief : 初始化窗口系统设置
**@param : none
**@return: none
*****************************/
void ManagementConnectClient::initSysSetting()
{
    manClientsTableWidget = new QTableWidget(ui->bodyWidget);
    manClientsTableWidget->setFixedSize(480, 311);

    manClientsTableWidget->setFocusPolicy(Qt::NoFocus);
    manClientsTableWidget->setObjectName("manClientsTable");
    manClientsTableWidget->move(10, 30);
    manClientsTableWidget->setColumnCount(4);
    //设置列宽
    manClientsTableWidget->setColumnWidth(0, 170);
    manClientsTableWidget->setColumnWidth(1, 125);
    manClientsTableWidget->setColumnWidth(2, 80);
    manClientsTableWidget->setColumnWidth(3, 80);
    manClientsTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList headList;
    headList << tr("客户端IP") << tr("端口号") << tr("屏蔽发送") << tr("屏蔽接收");
    manClientsTableWidget->setHorizontalHeaderLabels(headList);

    QRegExp portRegx("[1-9]+");
    QValidator *disBtnValidator = new QRegExpValidator(portRegx, ui->disconnectLineEdit);
    ui->disconnectLineEdit->setValidator(disBtnValidator);

    connect(manClientsTableWidget,&QTableWidget::itemClicked,this,[=](){
        int row = manClientsTableWidget->currentRow();
        ui->disconnectLineEdit->setText(QString::number(row+1));
    });
}

/*****************************
**@brief : 加载样式表
**@param : sheetName 样式表名称
**@return: none
*****************************/
void ManagementConnectClient::loadStyleSheet(const QString &sheetName)
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
**@brief : 断开连接按钮槽函数
**@param : none
**@return: none
*****************************/
void ManagementConnectClient::on_disconnectBtn_clicked()
{
    QString row = ui->disconnectLineEdit->text();
    ui->disconnectLineEdit->clear();
    emit disconnectToClient(row);
}
