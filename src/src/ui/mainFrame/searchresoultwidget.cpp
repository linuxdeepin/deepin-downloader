#include "searchresoultwidget.h"
#include <QListWidgetItem>
#include <QWidget>

SearchResoultWidget::SearchResoultWidget(QWidget *parent)
    : QListWidget(parent)
{
    setFixedHeight(300);
}

void SearchResoultWidget::setData(int currentTab, QString text)
{
    QListWidgetItem *item = new QListWidgetItem;
    item->setIcon(QIcon::fromTheme("dcc_list_downloading"));
    item->setText("正在下載->test.zip");
    addItem(item);
}
