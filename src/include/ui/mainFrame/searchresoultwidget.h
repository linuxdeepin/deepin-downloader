#ifndef SEARCHRESOULTWIDGET_H
#define SEARCHRESOULTWIDGET_H

#include <DListWidget>
#include <DDialog>

DWIDGET_USE_NAMESPACE

class SearchResoultWidget : public QListWidget
{
public:
    SearchResoultWidget(QWidget *parent = nullptr);

    void setData(QList<QString> &taskIDList,
                 QList<int> &taskStatusList, QList<QString> &tasknameList);
protected:
    void focusOutEvent(QFocusEvent *event);
};
#endif // SEARCHRESOULTWIDGET_H
