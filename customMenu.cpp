#include "customMenu.h"

CustomMenu::CustomMenu(QWidget *parent)
    : QMenu(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

CustomMenu::~CustomMenu()
{}

void CustomMenu::addCustomMenu(const QString & text, const QString & icon, const QString & name)
{
    QAction* pAction = addAction(QIcon(icon), name);
    m_menuActionMap.insert(text, pAction);
}

QAction* CustomMenu::getAction(const QString& text)
{
    return m_menuActionMap[text];
}
