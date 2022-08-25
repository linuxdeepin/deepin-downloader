/****************************************************************************
** Meta object code from reading C++ file 'downloadsettingwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../src/include/ui/settings/downloadsettingwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'downloadsettingwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DownloadSettingWidget_t {
    QByteArrayData data[12];
    char stringdata0[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DownloadSettingWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DownloadSettingWidget_t qt_meta_stringdata_DownloadSettingWidget = {
    {
QT_MOC_LITERAL(0, 0, 21), // "DownloadSettingWidget"
QT_MOC_LITERAL(1, 22, 21), // "speedLimitInfoChanged"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 4), // "text"
QT_MOC_LITERAL(4, 50, 14), // "onValueChanged"
QT_MOC_LITERAL(5, 65, 3), // "var"
QT_MOC_LITERAL(6, 69, 13), // "onTimeChanged"
QT_MOC_LITERAL(7, 83, 4), // "time"
QT_MOC_LITERAL(8, 88, 13), // "onTextChanged"
QT_MOC_LITERAL(9, 102, 20), // "onRadioButtonClicked"
QT_MOC_LITERAL(10, 123, 14), // "onFocusChanged"
QT_MOC_LITERAL(11, 138, 7) // "onFocus"

    },
    "DownloadSettingWidget\0speedLimitInfoChanged\0"
    "\0text\0onValueChanged\0var\0onTimeChanged\0"
    "time\0onTextChanged\0onRadioButtonClicked\0"
    "onFocusChanged\0onFocus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DownloadSettingWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   47,    2, 0x0a /* Public */,
       6,    1,   50,    2, 0x08 /* Private */,
       8,    1,   53,    2, 0x08 /* Private */,
       9,    0,   56,    2, 0x08 /* Private */,
      10,    1,   57,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QVariant,    5,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   11,

       0        // eod
};

void DownloadSettingWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DownloadSettingWidget *_t = static_cast<DownloadSettingWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->speedLimitInfoChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->onValueChanged((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 2: _t->onTimeChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->onTextChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->onRadioButtonClicked(); break;
        case 5: _t->onFocusChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DownloadSettingWidget::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DownloadSettingWidget::speedLimitInfoChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DownloadSettingWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DownloadSettingWidget.data,
      qt_meta_data_DownloadSettingWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DownloadSettingWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DownloadSettingWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DownloadSettingWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DownloadSettingWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void DownloadSettingWidget::speedLimitInfoChanged(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
