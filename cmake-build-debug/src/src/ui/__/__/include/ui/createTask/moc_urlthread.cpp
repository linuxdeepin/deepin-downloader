/****************************************************************************
** Meta object code from reading C++ file 'urlthread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../../../src/include/ui/createTask/urlthread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'urlthread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UrlThread_t {
    QByteArrayData data[10];
    char stringdata0[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UrlThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UrlThread_t qt_meta_stringdata_UrlThread = {
    {
QT_MOC_LITERAL(0, 0, 9), // "UrlThread"
QT_MOC_LITERAL(1, 10, 15), // "sendFinishedUrl"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 8), // "LinkInfo"
QT_MOC_LITERAL(4, 36, 4), // "link"
QT_MOC_LITERAL(5, 41, 11), // "sendTrueUrl"
QT_MOC_LITERAL(6, 53, 5), // "begin"
QT_MOC_LITERAL(7, 59, 13), // "onHttpRequest"
QT_MOC_LITERAL(8, 73, 14), // "QNetworkReply*"
QT_MOC_LITERAL(9, 88, 5) // "reply"

    },
    "UrlThread\0sendFinishedUrl\0\0LinkInfo\0"
    "link\0sendTrueUrl\0begin\0onHttpRequest\0"
    "QNetworkReply*\0reply"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UrlThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       5,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   40,    2, 0x08 /* Private */,
       7,    1,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

void UrlThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        UrlThread *_t = static_cast<UrlThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendFinishedUrl((*reinterpret_cast< LinkInfo(*)>(_a[1]))); break;
        case 1: _t->sendTrueUrl((*reinterpret_cast< LinkInfo(*)>(_a[1]))); break;
        case 2: _t->begin(); break;
        case 3: _t->onHttpRequest((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UrlThread::*)(LinkInfo );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UrlThread::sendFinishedUrl)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UrlThread::*)(LinkInfo );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UrlThread::sendTrueUrl)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UrlThread::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_UrlThread.data,
      qt_meta_data_UrlThread,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *UrlThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UrlThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UrlThread.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UrlThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void UrlThread::sendFinishedUrl(LinkInfo _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UrlThread::sendTrueUrl(LinkInfo _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
