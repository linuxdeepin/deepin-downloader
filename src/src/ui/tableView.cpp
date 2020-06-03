/**
 * @file tableView.cpp
 * @brief 下载条目列表
 * @author zhaoyue  <zhaoyue@uniontech.com>
 * @version 1.0.0
 * @date 2020-05-26 09:41
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 */

#include "tableView.h"
#include <QDebug>
#include <QScrollBar>
#include <QMouseEvent>
#include <QHeaderView>
#include <QModelIndex>
#include <QJsonObject>
#include <QDateTime>
#include <QProcess>
#include <QThread>
#include "../database/dbinstance.h"
#include "global.h"
#include "../aria2/aria2rpcinterface.h"
#include "tableModel.h"
#include "headerView.h"
#include "itemDelegate.h"
#include "settings.h"
#include "topButton.h"


TableView::TableView(int Flag, TopButton* pToolBar)
    : QTableView()
    , m_iTableFlag(Flag)
    , m_pTableModel(new TableModel(Flag))
    , m_pSetting(Settings::getInstance())
    , m_pToolBar(pToolBar)
{
    initUI();
}

void TableView::initUI()
{
    setModel(m_pTableModel);
    m_pItemdegegate = new ItemDelegate(this, Table_Flag);
    setItemDelegate(m_pItemdegegate);
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(636);
    setMouseTracking(true);

    // this->setMaximumWidth(2000);
    verticalHeader()->hide();

    // this->verticalHeader()->setDefaultSectionSize(56);

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    // setSelectionMode(QAbstractItemView::ExtendedSelection);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setAlternatingRowColors(true);
    setShowGrid(false);

    setSelectionMode(QAbstractItemView::SingleSelection);

    HeaderView *pHeaderView = new  HeaderView(Qt::Horizontal, this);
    setHorizontalHeader(pHeaderView);
    pHeaderView->setDefaultSectionSize(20);
    pHeaderView->setSortIndicatorShown(false);
    pHeaderView->setDefaultAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    pHeaderView->setSectionResizeMode(0, QHeaderView::Fixed);
    pHeaderView->setSectionResizeMode(1, QHeaderView::Stretch);
    pHeaderView->setSectionResizeMode(2, QHeaderView::Stretch);
    pHeaderView->setSectionResizeMode(3, QHeaderView::Stretch);
    pHeaderView->setSectionResizeMode(4, QHeaderView::Stretch);
    setColumnWidth(0, 20);

    connect(pHeaderView,
            &HeaderView::get_stateChanged,
            this,
            &TableView::header_stateChanged);
    connect(this,
            &TableView::clear_header_check,
            pHeaderView,
            &HeaderView::get_clear_header_check);
    connect(m_pTableModel,
            &TableModel::tableView_allChecked_or_allUnchecked,
            this,
            &TableView::get_tableview_allchecked);
    connect(this,
            &TableView::get_tableview_allchecked,
            pHeaderView,
            &HeaderView::get_checkall_signals);
    connect(this,
            &TableView::signal_hoverChanged,
            m_pItemdegegate,
            &ItemDelegate::slot_hoverChanged);

}

void TableView::initConnections()
{
}

void TableView::initTableView()
{
}

void TableView::setTestData()
{
    DataItem *data = new DataItem;

    data->taskId = "id";
    data->gid = "71bdc01777d598df";
    data->Ischecked = 0;
    data->fileName = "test.txt";
    data->taskId = "{8ffd889b-c0f9-4413-bf11-e98fe9ffc707}";
    data->createTime = "2020-05-22 15:39:14";
    data->Ischecked = false;
    getTableModel()->append(data);
    TableModel *dtModel = getTableModel();
    setRowHidden(dtModel->rowCount(QModelIndex()), true);
}

void TableView::reset(bool switched)
{
    QModelIndex idx = this->selectionModel()->currentIndex();
    int size = QTableView::verticalScrollBar()->value();

    QTableView::reset();

    this->selectRow(idx.row());
    if(switched) {
        size = 0;
    }
    QTableView::verticalScrollBar()->setValue(size);
}

void TableView::mousePressEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton) {
        setCurrentIndex(QModelIndex());
        QTableView::mousePressEvent(event);
        QModelIndex index = currentIndex();
        if((index.row() < 0) && (index.column() < 0)) {
            return;
        }
    }
}

TableModel * TableView::getTableModel()
{
    return m_pTableModel;
}

void TableView::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex idx = this->indexAt(event->pos());
    emit signal_hoverChanged(idx);
}

void TableView::leaveEvent(QEvent *event)
{
    this->reset();
    emit signal_hoverChanged(QModelIndex());
}

void TableView::keyPressEvent(QKeyEvent *event)
{

    if((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C)) {
        return;
    }
    QWidget::keyPressEvent(event);
}

void TableView::aria2MethodAdd(QJsonObject &json, QString &searchContent)
{
    QString id = json.value("id").toString();
    QString gId = json.value("result").toString();

    DataItem *finddata = getTableModel()->find(id);

    if(finddata != nullptr) {
        finddata->gid = gId;
        finddata->taskId = id;
        QDateTime finish_time = QDateTime::fromString("", "yyyy-MM-dd hh:mm:ss");
        S_Task_Status  get_status;
        S_Task_Status *download_status = new S_Task_Status(finddata->taskId,
                                                           Global::Status::Active,
                                                           QDateTime::currentDateTime(),
                                                           finddata->completedLength,
                                                           finddata->speed,
                                                           finddata->totalLength,
                                                           finddata->percent,
                                                           finddata->total,
                                                           finish_time);


        S_Task_Status task;
        DBInstance::getTaskStatusById(finddata->taskId, task);
        if(task.m_taskId != "") {
            DBInstance::updateTaskStatusById(*download_status);
        } else {
            DBInstance::addTaskStatus(*download_status);
        }
        finddata->status = Global::Status::Active;
    } else {
        // 获取下载信息
        // aria2c->tellStatus(gId, gId);
        Aria2RPCInterface::Instance()->getFiles(gId, id);
        DataItem *data = new DataItem;
        data->taskId = id;
        data->gid = gId;
        data->Ischecked = 0;
        QDateTime time = QDateTime::currentDateTime();
        data->createTime = time.toString("yyyy-MM-dd hh:mm:ss");

        S_Task getTaskInfo;
        S_Task taskInfo;
        DBInstance::getTaskByID(id, getTaskInfo);
        S_Task *task;
        if(getTaskInfo.m_taskId != "") {
            task = new S_Task(getTaskInfo.m_taskId,
                              gId,
                              0,
                              getTaskInfo.m_url,
                              getTaskInfo.m_downloadPath,
                              getTaskInfo.m_downloadFilename,
                              time);
            DBInstance::updateTaskByID(*task);
            data->fileName = getTaskInfo.m_downloadFilename;
        } else {
            task = new S_Task(id, gId, 0, "", "", "Unknow", time);
            DBInstance::addTask(*task);
        }
        getTableModel()->append(data);
        if((searchContent != "") && !data->fileName.contains(searchContent)) {
            TableModel *dtModel = getTableModel();
            setRowHidden(dtModel->rowCount(QModelIndex()), true);
        }
    }
}

void TableView::aria2MethodStatusChanged(QJsonObject &json, int iCurrentRow, QString &searchContent)
{
    QJsonObject result = json.value("result").toObject();
    QJsonObject bittorrent = result.value("bittorrent").toObject();
    QString     mode;
    QString     bittorrent_name;
    QString     taskId = json.value("id").toString();
    QString     bittorrent_dir = "";

    if(!bittorrent.isEmpty()) {
        mode = bittorrent.value("mode").toString();
        if(mode == "multi") {
            bittorrent_dir = result.value("dir").toString();
        }
        QJsonObject btInfo = bittorrent.value("info").toObject();
        bittorrent_name = btInfo.value("name").toString();
        QString infoHash = result.value("infoHash").toString();
        S_Url_Info tbUrlInfo;
        S_Url_Info getUrlInfo;
        DBInstance::getUrlById(taskId, getUrlInfo);
        if(getUrlInfo.m_taskId != "") {
            if(getUrlInfo.m_infoHash.isEmpty()) {
                S_Url_Info *url_info = new S_Url_Info(getUrlInfo.m_taskId,
                                                      getUrlInfo.m_url,
                                                      getUrlInfo.m_downloadType,
                                                      getUrlInfo.m_seedFile,
                                                      getUrlInfo.m_selectedNum,
                                                      infoHash);
                DBInstance::updateUrlById(*url_info);
            }
        }
    }
    QJsonArray files = result.value("files").toArray();

    QString filePath;
    QString fileUri;
    for(int i = 0; i < files.size(); ++i) {
        QJsonObject file = files[i].toObject();
        filePath = file.value("path").toString();
        QJsonArray uri = file.value("uris").toArray();
        for(int j = 0; j < uri.size(); ++j) {
            QJsonObject uriObject = uri[j].toObject();
            fileUri = uriObject.value("uri").toString();
        }
    }

    QString gId = result.value("gid").toString();

    long totalLength = result.value("totalLength").toString().toLong();         //
                                                                                //
                                                                                // 字节
    long completedLength = result.value("completedLength").toString().toLong(); //
                                                                                //
                                                                                // 字节
    long downloadSpeed = result.value("downloadSpeed").toString().toLong();     //
                                                                                //
                                                                                // 字节/每秒
    QString fileName = getFileName(filePath);
    QString statusStr = result.value("status").toString();

    int percent = 0;
    int status = 0;

    if((completedLength != 0) && (totalLength != 0)) {
        double tempPercent = completedLength * 100.0 / totalLength;
        percent = tempPercent;
        if((percent < 0) || (percent > 100)) {
            percent = 0;
        }
        if(completedLength == totalLength) {
            statusStr = "complete";
        }
    }

    if(statusStr == "active") {
        status = Global::Status::Active;
    } else if(statusStr == "waiting") {
        status = Global::Status::Waiting;
    } else if(statusStr == "paused") {
        status = Global::Status::Paused;
    } else if(statusStr == "error") {
        status = Global::Status::Error;
        dealNotificaitonSettings(statusStr, fileName);
    } else if(statusStr == "complete") {
        status = Global::Status::Complete;

        //下载文件为种子文件
        if(fileName.endsWith(".torrent")) {
            emit signalAutoDownloadBt(filePath);
        }

        //下载文件为磁链种子文件
        QString infoHash = result.value("infoHash").toString();
        if(filePath.startsWith("[METADATA]")) {
            QString dir = result.value("dir").toString();

            emit signalAutoDownloadBt(dir + "/" + infoHash + ".torrent");
            fileName = infoHash + ".torrent";
        }

        //
        dealNotificaitonSettings(statusStr, fileName);
    } else if(statusStr == "removed") {
        status = Global::Status::Removed;
    }

    DataItem *data = getTableModel()->find(taskId);
    if(data == nullptr) {
        return;
    }
    data->gid = gId;
    data->totalLength = formatFileSize(totalLength);
    data->completedLength = formatFileSize(completedLength);
    data->speed = (downloadSpeed != 0) ? formatDownloadSpeed(downloadSpeed) : "0kb/s";

    if(bittorrent.isEmpty()) {
        if(!fileName.isEmpty() && (data->fileName != fileName)) {
            data->fileName = fileName;
        }

        //                if(data->fileName==QObject::tr("Unknown"))
        //                {
        //                    data->fileName = (fileName.isEmpty()) ?
        // Global::UNKNOWN : fileName;
        //                }
        data->status = status;
    } else {
        // data->fileName = (bittorrent_name.isEmpty()) ? Global::UNKNOWN :
        // bittorrent_name;
        if(mode == "multi") {
            filePath = bittorrent_dir + "/" + bittorrent_name;
        }
        if((totalLength != 0) && (totalLength == completedLength)) {
            data->status = Complete;
            dealNotificaitonSettings("complete", filePath);
        } else {
            data->status = status;
        }

        fileUri = "";
    }
    data->percent = percent;
    data->total = totalLength;
    if(filePath != "") {
        data->savePath = filePath;
    } else {
        data->savePath = getDownloadSavepathFromConfig();
    }

    data->url = fileUri;
    data->time = "";

    if((totalLength != completedLength) && (totalLength != 0) &&
       (data->status == Global::Status::Active)) {
        QTime t(0, 0, 0);
        t = t.addSecs((totalLength - completedLength * 1.0) / downloadSpeed);
        data->time = t.toString("mm:ss");
    } else if((totalLength == 0) && (data->status == Global::Status::Active)) {
        data->time = ("--:--");
    } else {
        if(data->time == "") {
            data->time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        }

        //              updatetimer->stop();
    }
    S_Task task;
    S_Task getTask;
    DBInstance::getTaskByID(data->taskId, getTask);
    if(getTask.m_taskId != "") {
        if(getTask.m_url != "") {
            data->url = getTask.m_url;
        }
    }

    update();
    reset();
    S_Task_Status taskStatus;
    S_Task_Status getTaskStatus;
    DBInstance::getTaskStatusById(data->taskId, getTaskStatus);

    QDateTime get_time = QDateTime::fromString(data->time, "yyyy-MM-dd hh:mm:ss");
    S_Task_Status *save_task_status = new S_Task_Status(data->taskId,
                                                        data->status,
                                                        get_time,
                                                        data->completedLength,
                                                        data->speed,
                                                        data->totalLength,
                                                        data->percent,
                                                        data->total,
                                                        get_time);
    if(getTaskStatus.m_taskId != "") {
        DBInstance::addTaskStatus(*save_task_status);
    } else {
        if(getTaskStatus.m_downloadStatus != data->status) {
            DBInstance::updateTaskStatusById(*save_task_status);
        }
    }
    refreshTableView(iCurrentRow);
    if((data->status == Complete) && (searchContent != "")) {
        searchEditTextChanged(searchContent);
    }
}

void TableView::aria2MethodShutdown(QJsonObject &json)
{
    QString result = json.value("result").toString();
    if(result == "OK") {
        //m_bShutdownOk = true;
        qDebug() << "close downloadmanager";
        this->close();
        DApplication::exit();
    }
}

void TableView::aria2MethodGetFiles(QJsonObject &json, int iCurrentRow)
{
    QString   id = json.value("id").toString();
    DataItem *data = getTableModel()->find(id);
    if(data == nullptr) {// id等于gid
        data = new DataItem();
        QJsonArray  ja = json.value("result").toArray();
        QJsonObject jo = ja.at(0).toObject();
        data->totalLength = jo.value("length").toString().toLong(); // 文件大小
        data->savePath = jo.value("path").toString();               //下载路径，带文件名
        data->fileName = data->savePath.mid(data->savePath.lastIndexOf('/') + 1);
        QJsonArray uris = jo.value("uris").toArray();
        data->url = uris.at(0).toObject().value("uri").toString();  //下载链接
        data->taskId = id;
        getTableModel()->append(data);
    }
    reset();
    refreshTableView(iCurrentRow);
}

void TableView::aria2MethodUnpause(QJsonObject &json, int iCurrentRow)
{
    QString gId = json.value("result").toString();
    QString taskId = json.value("id").toString();

    DataItem *data = getTableModel()->find(taskId);
    if(data != nullptr) {
        data->status = Global::Status::Active;
        refreshTableView(iCurrentRow);
    }
}

void TableView::aria2MethodForceRemove(QJsonObject &json)
{
    QString id = json.value("id").toString();
    if(id.startsWith("REDOWNLOAD_")) { // 重新下载前的移除完成后
        QStringList sp = id.split("_");
        QString     taskId = sp.at(2);
        int rd = sp.at(1).toInt();
        QThread::msleep(500);
        emit signalRedownload(taskId, rd);
    }
}

QString TableView::getFileName(const QString &url)
{
    return QString(url).right(url.length() - url.lastIndexOf('/') - 1);
}

void TableView::dealNotificaitonSettings(QString statusStr, QString fileName)
{
    // 获取免打扰模式值
    QVariant undisturbed_mode_switchbutton = m_pSetting->m_pSettings->getOption(
        "basic.select_multiple.undisturbed_mode_switchbutton");

    if(undisturbed_mode_switchbutton.toBool()) {
        bool topStatus = this->isTopLevel();
        bool maxStatus = this->isMaximized();
        if(topStatus && maxStatus) {
            return;
        }
    }

    QVariant afterDownloadPlayTone = m_pSetting->m_pSettings->getOption("noticeReminder.remind.afterDownloadPlayTone");
    if(afterDownloadPlayTone.toBool()) {
        // QSound::play(":/resources/wav/downloadfinish.wav");
    } else {
        qDebug() << " not in select down load finsh wav" << endl;
    }

    QVariant downloadInfoNotify = m_pSetting->m_pSettings->getOption("noticeReminder.remind.downloadInfoNotify");
    if(downloadInfoNotify.toBool()) {
        QProcess *p = new QProcess;
        QString   showInfo;
        if(statusStr == "error") {
            showInfo = fileName + tr(" download error");
        } else {
            showInfo = fileName + tr(" download complete");
        }
        p->start("notify-send", QStringList() << showInfo);
        p->waitForStarted();
        p->waitForFinished();
    }
}

QString TableView::formatFileSize(long size)
{
    QString result = "";

    if(size < 1024) {
        result = QString::number(size) + "B";
    } else if(size / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024, 'r', 1) + "KB";
    } else if(size / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024, 'r', 1) + "MB";
    } else if(size / 1024 / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024 / 1024, 'r', 1) + "GB";
    }

    return result;
}

QString TableView::getDownloadSavepathFromConfig()
{
    QVariant downloadRadioGroup =
        m_pSetting->m_pSettings->getOption("basic.downloadDirectory.downloadDirectoryFileChooser");

    QString path = "";

    QString currentValue = downloadRadioGroup.toString();

    // 无设置信息
    if(currentValue.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
    } else {
        // 用户设置默认目录
        if(currentValue.contains("custom;")) {
            path = currentValue.section(QString(';'), 1, 1);
            if(path.isEmpty()) {
                path =  QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
            }
        }
        // 自动修改为上次使用目录
        else {
            QString config_path = QString("%1/%2/%3/last_save_path")
                                  .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                                  .arg(qApp->organizationName())
                                  .arg(qApp->applicationName());
            QFile file;
            file.setFileName(config_path);
            if(file.exists()) {
                bool isOK = file.open(QIODevice::ReadOnly);
                if(isOK == true) {
                    QByteArray array = file.readAll();

                    path = array;
                } else {
                    path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
                }
                file.close();
            } else {
                path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QString("/Downloads");
            }
        }
    }
    return path;
}

QString TableView::formatDownloadSpeed(long size)
{
    QString result = "";

    if(size < 1024) {
        result = QString::number(size) + " B/s";
    } else if(size / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024, 'r', 1) + " KB/s";
    } else if(size / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024, 'r', 1) + " MB/s";
    } else if(size / 1024 / 1024 / 1024 < 1024) {
        result = QString::number(size * 1.0 / 1024 / 1024 / 1024, 'r', 1) + " GB/s";
    }

    return result;
}

void TableView::refreshTableView(const int &index)
{
    switch(index) {
    case 0: {
        getTableModel()->switchDownloadingMode();
        setColumnHidden(3, false);
        setColumnHidden(4, true);

        // 联动工具栏按钮 begin
        int chkedCnt = 0;
        QList<DataItem *> selectList = getTableModel()->renderList();
        for(int i = 0; i < selectList.size(); i++) {
            if(selectList.at(i)->Ischecked) {
                chkedCnt++;
            }
        }
        if(chkedCnt > 0) {
            m_pToolBar->enableStartBtn(true);
            m_pToolBar->enablePauseBtn(true);
            m_pToolBar->enableDeleteBtn(true);
        } else {
            m_pToolBar->enableStartBtn(false);
            m_pToolBar->enablePauseBtn(false);
            m_pToolBar->enableDeleteBtn(false);
        }
        break;
    }

    case 1:
        getTableModel()->switchFinishedMode();

        setColumnHidden(3, true);
        setColumnHidden(4, false);
        break;
    }


    update();
}

void TableView::searchEditTextChanged(QString text)
{
    TableModel *pModel = getTableModel();
    if(text == "") {
        for(int i = 0; i < pModel->rowCount(QModelIndex()); i++) {
            setRowHidden(i, false);
            pModel->setData(pModel->index(i, 0), false, TableModel::Ischecked);
        }
    } else {
        for(int i = 0; i < pModel->rowCount(QModelIndex()); i++) {
            setRowHidden(i, false);
            QString fileName = pModel->data(pModel->index(i, 1), TableModel::FileName).toString();
            if(!fileName.contains(text, Qt::CaseInsensitive)) {
                setRowHidden(i, true);
            }
            pModel->setData(pModel->index(i, 0), false, TableModel::Ischecked);
        }
    }
    reset();
}
