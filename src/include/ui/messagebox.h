#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H
#include <DDialog>
#include <DLabel>
#include <DTextEdit>
#include <DCheckBox>
#include <DLineEdit>
#include <QPushButton>
#include "global.h"


DWIDGET_USE_NAMESPACE

class MessageBox : public DDialog
{
    Q_OBJECT
public:
    explicit MessageBox(DDialog *parent = nullptr);

public:
    void setWarings(QString warningMsg,QString surebtntext,QString cancalbtntext="",int sameurl_count=0,QList<QString> same_url_list={});
    void setDelete(bool permanentl);
    void setClear();
    void setReName(QString title,QString surebtntext,QString cancelbtn,QString oldname);
private:
    void addLabel(QString text);
    void addCheckbox(QString checkboxText);
signals:
    void ReName_sig(QString newname);
    void ClearRecycle_sig(bool);
    void DeleteDownload_sig(bool,bool);

public slots:
    void get_renameLineEdit_changed(const QString &text);
    void renameSureBtn();
    void clearBtn(int index);
    void deleteBtn(int index);

private:
    bool m_deleteFlag;
    DCheckBox *m_checkbox;
    DLineEdit *newName_lineedit;
    QPushButton *rename_sure_button;

};

#endif // MESSAGEBOX_H
