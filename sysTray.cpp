#include "sysTray.h"
#include  "customMenu.h"

SysTray::SysTray(QWidget* parent)
    : m_parent(parent), QSystemTrayIcon(parent)
{
    initSystemTray();
    qDebug() << "systray init over";
    this->show();
    qDebug() << "systray show over";
}

SysTray::~SysTray()
{
}

void SysTray::initSystemTray()
{
    setToolTip(QStringLiteral("网络调试助手"));
    setIcon(QIcon(":/Resources/imageIcon/networkAssistant.ico"));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
        SLOT(onIconActivated(QSystemTrayIcon::ActivationReason)));
}

void SysTray::addSystrayMenu()
{
    CustomMenu* customMenu = new CustomMenu(m_parent);
    customMenu->addCustomMenu("onShow", ":/Resources/imageIcon/networkAssistant.ico", QStringLiteral("显示"));
    customMenu->addCustomMenu("onQuit", ":/Resources/imageIcon/networkAssistant.ico", QStringLiteral("关闭"));

    connect(customMenu->getAction("onShow"), SIGNAL(triggered(bool)), m_parent,
        SLOT(onShowNormal(bool)));
    connect(customMenu->getAction("onQuit"), SIGNAL(triggered(bool)), m_parent,
        SLOT(onShowQuit(bool)));

    customMenu->exec(QCursor::pos());
    delete customMenu;
    customMenu = nullptr;
}

void SysTray::onIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger){
        m_parent->show();
        m_parent->activateWindow();
    }
    else if (reason == QSystemTrayIcon::Context) {
        addSystrayMenu();
    }
}
