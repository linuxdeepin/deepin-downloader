/****************************************************************************
** Meta object code from reading C++ file 'analysisurl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../../../src/include/ui/createTask/analysisurl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'analysisurl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_AnalysisUrl_t {
    QByteArrayData data[8];
    char stringdata0[85];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AnalysisUrl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AnalysisUrl_t qt_meta_stringdata_AnalysisUrl = {
    {
QT_MOC_LITERAL(0, 0, 11), // "AnalysisUrl"
QT_MOC_LITERAL(1, 12, 15), // "sendFinishedUrl"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 9), // "LinkInfo*"
QT_MOC_LITERAL(4, 39, 8), // "linkInfo"
QT_MOC_LITERAL(5, 48, 11), // "getLinkInfo"
QT_MOC_LITERAL(6, 60, 8), // "LinkInfo"
QT_MOC_LITERAL(7, 69, 15) // "getTrueLinkInfo"

    },
    "AnalysisUrl\0sendFinishedUrl\0\0LinkInfo*\0"
    "linkInfo\0getLinkInfo\0LinkInfo\0"
    "getTrueLinkInfo"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AnalysisUrl[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   32,    2, 0x0a /* Public */,
       7,    1,   35,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    2,
    QMetaType::Void, 0x80000000 | 6,    2,

       0        // eod
};

void AnalysisUrl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AnalysisUrl *_t = static_cast<AnalysisUrl *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendFinishedUrl((*reinterpret_cast< LinkInfo*(*)>(_a[1]))); break;
        case 1: _t->getLinkInfo((*reinterpret_cast< LinkInfo(*)>(_a[1]))); break;
        case 2: _t->getTrueLinkInfo((*reinterpret_cast< LinkInfo(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (AnalysisUrl::*)(LinkInfo * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&AnalysisUrl::sendFinishedUrl)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject AnalysisUrl::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AnalysisUrl.data,
      qt_meta_data_AnalysisUrl,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *AnalysisUrl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AnalysisUrl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_AnalysisUrl.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AnalysisUrl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void AnalysisUrl::sendFinishedUrl(LinkInfo * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
