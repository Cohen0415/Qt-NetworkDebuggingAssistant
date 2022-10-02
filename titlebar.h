/*
 * 文件名：titlebar.h
 * 最后修改日期：2022.09.13
 * 作者：林政硕
 * 功能：通用标题栏
*/

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

enum netWindowType{
    mainWindows = 0,
    manClientsWindows
};

class TitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = nullptr, netWindowType type = mainWindows);
    ~TitleBar();

    /*****************************
    **@brief : 设置标题栏大小
    **@param : width 宽度
    **@param : hight 长度
    **@return: none
    *****************************/
    void setTitleBarSize(const unsigned int width, const unsigned int hight);

    /*****************************
    **@brief : 设置标题栏标语
    **@param : slogan 标语
    **@return: none
    *****************************/
    void setTitleSlogan(const QString slogan);

signals:
    void quitMainWindow();

private slots:

    /*****************************
    **@brief : 最小化窗口
    **@param : none
    **@return: none
    *****************************/
    void onMinShow();

    /*****************************
    **@brief : 关闭窗口
    **@param : none
    **@return: none
    *****************************/
    void onCloseShow();

private:

    /*****************************
    **@brief : 鼠标按下事件
    **@param :
    **@return: none
    *****************************/
    void mousePressEvent(QMouseEvent* event);

    /*****************************
    **@brief : 鼠标松开事件
    **@param :
    **@return: none
    *****************************/
    void mouseReleaseEvent(QMouseEvent* event);

    /*****************************
    **@brief : 鼠标移动事件
    **@param :
    **@return: none
    *****************************/
    void mouseMoveEvent(QMouseEvent* event);

private:

    /*****************************
    **@brief : 初始化标题栏
    **@param : none
    **@return: none
    *****************************/
    void initTitleBar();

    /*****************************
    **@brief : 加载样式表
    **@param : sheetName 样式表名称
    **@return: none
    *****************************/
    void loadStyleSheet(const QString &sheetName);

private:
    QWidget *m_parent;
    QWidget *titleWidget;           //标题widget
    netWindowType wType;

    QLabel *m_iconLabel;            //图标
    QPushButton *m_sysMinBtn;       //最小化按钮
    QPushButton *m_sysCloseBtn;     //关闭按钮
    QPushButton *m_packUpBtn;       //收起按钮
    QLabel *m_titleSloganLabel;     //标题栏标语

    unsigned int m_titleBarWidth;   //标题栏宽
    unsigned int m_titleBarHight;   //标题栏高
    QString m_titleBarSlogan;       //标题栏标语

    bool mousePressedFlag;          //鼠标按下标志位
    QPoint m_startMoveMousePos;     //窗口开始移动时鼠标初始位置
};

#endif // TITLEBAR_H
