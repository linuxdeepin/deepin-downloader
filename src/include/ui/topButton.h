#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <DIconButton>
#include <DPushButton>
#include <DSearchEdit>
#include <DLabel>


#include <QWidget>

DWIDGET_USE_NAMESPACE

class TopButton : public QWidget
{
    Q_OBJECT
public:
    explicit TopButton(QWidget *parent = nullptr);

    QString getSearchText()
    {
        return this->m_pSearchEdit->text();
    }

    void enableStartBtn(bool enable)
    {
        this->m_pStartDownloadBtn->setEnabled(enable);
    }

    void enablePauseBtn(bool enable)
    {
        this->m_pPauseDownloadBtn->setEnabled(enable);
    }

    void enableDeleteBtn(bool enable)
    {
        this->m_pDeleteDownloadBtn->setEnabled(enable);
    }

public slots:
    void get_table_changed(int index);
private:
    void Init();
    void InitConnections();

signals:
    void newDownloadBtnClicked();
    void pauseDownloadBtnClicked();
    void startDownloadBtnClicked();
    void deleteDownloadBtnClicked();
    void getSearchEditFocus(bool focus);
    void getSearchEditTextChange(QString text);
private:
    DLabel *m_pIconLable;
    DIconButton *m_pNewDownloadBtn;
    DIconButton *m_pStartDownloadBtn;
    DIconButton *m_pStopDownloadBtn;
    DIconButton *m_pPauseDownloadBtn;
    DIconButton *m_pDeleteDownloadBtn;
    DSearchEdit *m_pSearchEdit;
};

#endif // TOOLBAR_H
