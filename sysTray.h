#pragma once
#include <QWidget>
#include <QSystemTrayIcon>
#include <QDebug>

class SysTray  : public QSystemTrayIcon
{
    Q_OBJECT

public:
    SysTray(QWidget *parent);
    ~SysTray();

private slots:
    void onIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void initSystemTray();
    void addSystrayMenu();

private:
    QWidget* m_parent;

};
