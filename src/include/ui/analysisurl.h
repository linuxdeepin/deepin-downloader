#ifndef ANALYSISURL_H
#define ANALYSISURL_H

#include "urlthread.h"
#include <QObject>
#include <QMap>


struct LinkInfo
{
    enum UrlState{
        Start = 1,
        Analysising,
        Finished
    };
    int index;              //url序号
    QString url;            //url链接
    UrlState state;         //url状态
    QString urlName;        //url名字
    QString type;           //url类型
    QString urlSize;        //url大小
    QString urlTrueLink;    //url真实链接
    long length;
    LinkInfo(){
        index = -1;
        length = 0;
        url.clear();
        state = UrlState::Start;
        urlName.clear();
        urlSize.clear();
        urlTrueLink.clear();
        type.clear();
    }
    LinkInfo &operator = (const LinkInfo &l) {
        index = l.index;
        state = l.state;
        url = l.url;
        type = l.type;
        urlSize = l.urlSize;
        urlName = l.urlName;
        urlTrueLink = l.urlTrueLink;
        length = l.length;
        return *this;
    }
};

class AnalysisUrl : public QObject
{
    Q_OBJECT

public:
    explicit AnalysisUrl(QObject *parent = nullptr);
    ~AnalysisUrl();

    void setUrlList(QMap<QString, LinkInfo> list);

public slots:
    void getLinkInfo(LinkInfo);
private:
    void stopWork(int index);

signals:
    void sendFinishedUrl(LinkInfo*);

private:
    QMap<QString, LinkInfo> m_curAllUrl;
    QMap<int, QThread*> m_workThread;

};

#endif // ANALYSISURL_H
