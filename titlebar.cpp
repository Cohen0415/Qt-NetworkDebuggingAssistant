/*
 * 文件名：titlebar.h
 * 最后修改日期：2022.09.13
 * 作者：林政硕
 * 功能：通用标题栏
*/

#include "titlebar.h"
#include "sysTray.h"

#include <QHBoxLayout>
#include <QSpacerItem>
#include <QDebug>
#include <QFile>
#include <QMouseEvent>

TitleBar::TitleBar(QWidget *parent, netWindowType type) : QWidget(parent), m_parent(parent)
{
    wType = type;
    initTitleBar();
    loadStyleSheet("titleBar");
    show();
}

TitleBar::~TitleBar()
{
}

/*****************************
**@brief : 初始化标题栏
**@param : none
**@return: none
*****************************/
void TitleBar::initTitleBar()
{
    //初始化鼠标按下标志位
    mousePressedFlag = false;

    //设置标题栏样式
    setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);	//透明

    titleWidget = new QWidget(this);
    titleWidget->setObjectName("titleWidget");

    //设置图标
    m_iconLabel = new QLabel(titleWidget);
    m_iconLabel->setFixedSize(28, 28);
    m_iconLabel->setObjectName("appIcon");

    //初始化标题文本
    m_titleSloganLabel = new QLabel(titleWidget);
    m_titleSloganLabel->setToolTip("QQ: 1033878279");

    //初始化最小化、关闭按钮
    m_sysMinBtn = new QPushButton(titleWidget);
    m_sysCloseBtn = new QPushButton(titleWidget);
    m_sysMinBtn->setFixedSize(32, 32);
    m_sysCloseBtn->setFixedSize(32, 32);

    m_sysMinBtn->setObjectName("sysMinBtn");
    m_sysCloseBtn->setObjectName("sysCloseBtn");

    connect(m_sysMinBtn, &QPushButton::clicked,
            this, &TitleBar::onMinShow);
    connect(m_sysCloseBtn, &QPushButton::clicked,
            this, &TitleBar::onCloseShow);

    QHBoxLayout *titleBarLayout = new QHBoxLayout(titleWidget);
    QSpacerItem *hSpacer = new QSpacerItem(40, 20,
                                           QSizePolicy::Expanding,
                                           QSizePolicy::Fixed);
    titleBarLayout->addWidget(m_iconLabel);
    titleBarLayout->addWidget(m_titleSloganLabel);
    titleBarLayout->addItem(hSpacer);
    titleBarLayout->addWidget(m_sysMinBtn);
    titleBarLayout->addWidget(m_sysCloseBtn);
    titleBarLayout->setMargin(10);

    qDebug() << "titleBar init over...";
}

/*****************************
**@brief : 加载样式表
**@param : sheetName 样式表名称
**@return: none
*****************************/
void TitleBar::loadStyleSheet(const QString &sheetName)
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
**@brief : 设置标题栏大小
**@param : width 宽度
**@param : hight 长度
**@return: none
*****************************/
void TitleBar::setTitleBarSize(const unsigned int width, const unsigned int hight)
{
    m_titleBarWidth = width;
    m_titleBarHight = hight;
    setFixedSize(m_titleBarWidth, m_titleBarHight);
    titleWidget->setFixedSize(m_titleBarWidth, m_titleBarHight);
}

/*****************************
**@brief : 设置标题栏标语
**@param : slogan 标语
**@return: none
*****************************/
void TitleBar::setTitleSlogan(const QString slogan)
{
    m_titleBarSlogan = slogan;
    m_titleSloganLabel->setText(m_titleBarSlogan);
}

/*****************************
**@brief : 最小化窗口
**@param : none
**@return: none
*****************************/
void TitleBar::onMinShow()
{
    m_parent->showMinimized();
}

/*****************************
**@brief : 关闭窗口
**@param : none
**@return: none
*****************************/
void TitleBar::onCloseShow()
{
    if(wType == mainWindows){
        emit quitMainWindow();

    }
    else if(wType == manClientsWindows){
        m_parent->close();
    }

}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    mousePressedFlag = true;
    m_startMoveMousePos = event->globalPos();

    return QWidget::mousePressEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    mousePressedFlag = false;
    return QWidget::mouseReleaseEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePressedFlag){
        QPoint movePoint = event->globalPos() - m_startMoveMousePos;    //需要移动的距离
        QPoint titleWidgetPos = parentWidget()->pos();      //当前窗体的位置
        parentWidget()->move(titleWidgetPos.x()+movePoint.x(),
                             titleWidgetPos.y()+movePoint.y());

        m_startMoveMousePos = event->globalPos();
    }
    return QWidget::mouseMoveEvent(event);
}









