#ifndef SEARCHRESOULTWIDGET_H
#define SEARCHRESOULTWIDGET_H

#include <DListWidget>

class SearchResoultWidget : public QListWidget
{
public:
    SearchResoultWidget(QWidget *parent = nullptr);

    void setData(QList<QString> &taskIDList,
                 QList<int> &taskStatusList, QList<QString> &tasknameList);
};

#endif // SEARCHRESOULTWIDGET_H
