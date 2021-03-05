#pragma once

#include <DComboBox>
#include <DLineEdit>
#include <DListWidget>

#include <QTime>
#include <QTimeEdit>

DWIDGET_USE_NAMESPACE
class CTimeEdit : public DComboBox
{
    Q_OBJECT

public:
    CTimeEdit(QWidget *parent = nullptr);
    ~CTimeEdit();
    void setTime(QTime time);
    QTime getTime();
private:
    void initUI();
    void initConnection();
private slots:
    void onIndexChanged(const QString & text);
signals:
    void inputFinished(const QString & text);
private:
    DLineEdit *m_timeEdit = nullptr;
    int m_type = 0; // 0 h,1 m
    int m_pos = 0;
    QTime m_time;
};
