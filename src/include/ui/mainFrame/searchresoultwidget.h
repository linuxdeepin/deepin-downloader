#ifndef SEARCHRESOULTWIDGET_H
#define SEARCHRESOULTWIDGET_H

#include <DListWidget>

class SearchResoultWidget : public QListWidget
{
public:
    SearchResoultWidget(QWidget *parent = nullptr);

    void setData(int currentTab, QString text);
};

#endif // SEARCHRESOULTWIDGET_H
