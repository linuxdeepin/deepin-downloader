#include "searchresoultwidget.h"
#include <QListWidgetItem>
#include <QWidget>
#include <dplatformwindowhandle.h>
#include "global.h"

SearchResoultWidget::SearchResoultWidget(QWidget *parent)
    : QListWidget(parent)
{
    setMinimumWidth(380);
    setFixedHeight(200);
    DPlatformWindowHandle handle(this);
    handle.setWindowRadius(18);
}

void SearchResoultWidget::setData(QList<QString> &taskIDList,
                                  QList<int> &taskStatusList, QList<QString> &tasknameList)
{
    this->clear();

    for(int i = 0; i< taskIDList.count(); i++){
        QListWidgetItem *item = new QListWidgetItem;
        QString text;
        if(taskStatusList.at(i) == Global::Complete) {
            item->setIcon(QIcon::fromTheme("dcc_print_done"));
            text += tr("   Completed");
        } else if(taskStatusList.at(i) == Global::Removed) {
            item->setIcon(QIcon::fromTheme("dcc_list_delete"));
            text += tr("   Trash");
        } else {
            item->setIcon(QIcon::fromTheme("dcc_list_downloading"));
            text += tr("   Downloading");
        }

        item->setText(text + "  -->  " + tasknameList.at(i));
        item->setData(Qt::WhatsThisRole, taskIDList.at(i));
        addItem(item);
    }
}

void SearchResoultWidget::focusOutEvent(QFocusEvent *event)
{
    this->hide();
}
