/**
 * @copyright 2020-2020 Uniontech Technology Co., Ltd.
 *
 * @file Diagnostictool.cpp
 *
 * @brief 下载诊断工具
 *
 * @date 2020-08-24 17:10
 *
 * Author: zhaoyue  <zhaoyue@uniontech.com>
 *
 * Maintainer: zhaoyue  <zhaoyue@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "diagnostictool.h"
#include <DLabel>
#include <DGuiApplicationHelper>
#include <QLayout>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QTimer>
#include <QScrollBar>

#include "func.h"

DiagnosticTool::DiagnosticTool(DDialog *parent)
    : DDialog(parent)
    , m_Tableview(new QTableView)
    , m_Model(new DiagnosticModel)
{
    setFixedSize(453, 411);
    initUI();
    QTimer::singleShot(500, this, SLOT(startDiagnostic()));
}

void DiagnosticTool::initUI()
{
    QIcon tryIcon = QIcon(QIcon::fromTheme(":/icons/icon/downloader2.svg"));
    tryIcon.pixmap(QSize(30, 30));
    setIcon(tryIcon);

    QWidget *pWidget = new QWidget(this);
    pWidget->setFixedSize(453, 310);

    QFont f;
    f.setPixelSize(17);
    f.setBold(true);
    QLabel *pLabel = new QLabel(tr("Result:"));
    pLabel->setFixedSize(202, 25);
    pLabel->setFont(f);
    m_Button = new QPushButton(tr("Diagnose Again"), this);
    m_Button->setFixedSize(202, 36);
    connect(m_Button, &QPushButton::clicked, this, [=]() {
        m_Model->clearData();
        QTimer::singleShot(500, this, [=]() {
            startDiagnostic();
        });
    });
    QHBoxLayout *pHLayout = new QHBoxLayout(this);
    pHLayout->addStretch();
    pHLayout->addWidget(m_Button);
    pHLayout->addStretch();

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->addWidget(pLabel);
    pLayout->addWidget(m_Tableview);
    pLayout->addLayout(pHLayout);
    pWidget->setLayout(pLayout);
    addContent(pWidget, Qt::AlignHCenter);

    DiagnosticDelegate *pDelegate = new DiagnosticDelegate();
    m_Tableview->setModel(m_Model);
    m_Tableview->setItemDelegate(pDelegate);
    m_Tableview->horizontalHeader()->hide();
    m_Tableview->verticalHeader()->hide();
    m_Tableview->verticalHeader()->setDefaultSectionSize(40);
    m_Tableview->setFixedSize(418, 205);
    m_Tableview->setShowGrid(false);
    m_Tableview->setColumnWidth(0, 42);
    m_Tableview->setColumnWidth(1, 310);
    m_Tableview->setColumnWidth(2, 55);
    m_Tableview->setAlternatingRowColors(true);
    m_Tableview->setEnabled(false);
    m_Tableview->verticalScrollBar()->setHidden(true);
}

void DiagnosticTool::startDiagnostic()
{
    m_Button->setEnabled(false);
    //m_Model->appendData(Func::isIpv6Connect());
    m_Tableview->update();
    QTimer::singleShot(qrand() % (800) + 200, this, [=]() {
        m_Model->appendData(Func::isDHT());
    });

    QTimer::singleShot(qrand() % (800) + 800, this, [=]() {
        m_Model->appendData(Func::isHTTPConnect());
    });

    QTimer::singleShot(qrand() % (800) + 1400, this, [=]() {
        m_Model->appendData(Func::isBt());
    });

    QTimer::singleShot(qrand() % (800) + 2000, this, [=]() {
        m_Model->appendData(Func::isBt());
    });

    QTimer::singleShot(qrand() % (800) + 2500, this, [=]() {
        m_Model->appendData(Func::isNetConnect());
        m_Button->setEnabled(true);
    });
}

DiagnosticModel::DiagnosticModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

DiagnosticModel::~DiagnosticModel()
{
}

void DiagnosticModel::appendData(bool b)
{
    const int row = m_DiagnosticStatusList.size();
    beginInsertRows(QModelIndex(), row, row);
    m_DiagnosticStatusList.append(b);
    endInsertRows();
}

void DiagnosticModel::clearData()
{
    m_DiagnosticStatusList.clear();
}

bool DiagnosticModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    switch (role) {
    case Qt::DisplayRole:
        m_DiagnosticStatusList.replace(index.row(), value.toBool());
        break;
    default:
        break;
    }

    return true;
}

int DiagnosticModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

int DiagnosticModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant DiagnosticModel::data(const QModelIndex &index, int role) const
{
    if (m_DiagnosticStatusList.count() <= index.row()) {
        return QVariant();
    }
    switch (role) {
    case Qt::DisplayRole:
        if (index.column() == 0) {
            return m_DiagnosticStatusList.at(index.row()) ? (":icons/icon/normal.svg") : (":icons/icon/defeat.svg");
        } else if (index.column() == 1) {
            switch (index.row()) {
                //            case 0:
                //                return tr("IPv6 support");
            case 0:
                return tr("DHT status");
            case 1:
                return tr("HTTP task");
            case 2:
                return tr("BT task");
            case 3:
                return tr("Magnet task");
            case 4:
                return tr("Network detection");
            default:
                break;
            }
        } else if (index.column() == 2) {
            return m_DiagnosticStatusList.at(index.row()) ? tr("Pass") : tr("Failed");
        }
        break;
    case Qt::TextColorRole:
        return m_DiagnosticStatusList.at(index.row()) ? ("#00c77d") : ("#ff5736");
    }

    return QVariant();
}

DiagnosticDelegate::DiagnosticDelegate(QObject *parent, int Flag)
{
    Q_UNUSED(parent);
    Q_UNUSED(Flag);
}

DiagnosticDelegate::~DiagnosticDelegate()
{
}

void DiagnosticDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setPen(QColor(index.data(Qt::TextColorRole).toString()));
    if (index.row() % 2 == 0) {
        painter->fillRect(option.rect, Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().toolTipBase().color());
    } else {
        painter->fillRect(option.rect, QColor(245, 245, 245, 80));
    }

    switch (index.column()) {
    case 0: {
        QPixmap pic = index.data(Qt::DisplayRole).toString();
        QRect rect = QRect(option.rect.x() + 10, option.rect.y() + 10, 25, 25);
        painter->drawPixmap(rect, pic);
    } break;
    case 1: {
        QRect rect = QRect(option.rect.x(), option.rect.y() + 10, option.rect.width(), option.rect.height());
        painter->drawText(rect, index.data(Qt::DisplayRole).toString());
    } break;
    case 2: {
        QRect rect = QRect(option.rect.x(), option.rect.y() + 10, option.rect.width(), option.rect.height());
        painter->drawText(rect, index.data(Qt::DisplayRole).toString());
    } break;
    default:
        break;
    }
}
