/****************************************************************************
** Meta object code from reading C++ file 'messagebox.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../../src/include/ui/messagebox.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'messagebox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MessageBox_t {
    QByteArrayData data[23];
    char stringdata0[253];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MessageBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MessageBox_t qt_meta_stringdata_MessageBox = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MessageBox"
QT_MOC_LITERAL(1, 11, 6), // "Rename"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 8), // "QString&"
QT_MOC_LITERAL(4, 28, 7), // "newname"
QT_MOC_LITERAL(5, 36, 10), // "reDownload"
QT_MOC_LITERAL(6, 47, 7), // "sameUrl"
QT_MOC_LITERAL(7, 55, 8), // "fileName"
QT_MOC_LITERAL(8, 64, 4), // "type"
QT_MOC_LITERAL(9, 69, 12), // "Clearrecycle"
QT_MOC_LITERAL(10, 82, 3), // "ret"
QT_MOC_LITERAL(11, 86, 14), // "Deletedownload"
QT_MOC_LITERAL(12, 101, 4), // "ret1"
QT_MOC_LITERAL(13, 106, 12), // "closeConfirm"
QT_MOC_LITERAL(14, 119, 5), // "index"
QT_MOC_LITERAL(15, 125, 14), // "unusualConfirm"
QT_MOC_LITERAL(16, 140, 6), // "taskId"
QT_MOC_LITERAL(17, 147, 23), // "onRenamelineeditChanged"
QT_MOC_LITERAL(18, 171, 4), // "text"
QT_MOC_LITERAL(19, 176, 22), // "onRenameSureBtnClicked"
QT_MOC_LITERAL(20, 199, 17), // "onClearBtnClicked"
QT_MOC_LITERAL(21, 217, 18), // "onDeleteBtnClicked"
QT_MOC_LITERAL(22, 236, 16) // "onExitBtnClicked"

    },
    "MessageBox\0Rename\0\0QString&\0newname\0"
    "reDownload\0sameUrl\0fileName\0type\0"
    "Clearrecycle\0ret\0Deletedownload\0ret1\0"
    "closeConfirm\0index\0unusualConfirm\0"
    "taskId\0onRenamelineeditChanged\0text\0"
    "onRenameSureBtnClicked\0onClearBtnClicked\0"
    "onDeleteBtnClicked\0onExitBtnClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MessageBox[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       5,    3,   72,    2, 0x06 /* Public */,
       9,    1,   79,    2, 0x06 /* Public */,
      11,    2,   82,    2, 0x06 /* Public */,
      13,    1,   87,    2, 0x06 /* Public */,
      15,    2,   90,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      17,    1,   95,    2, 0x0a /* Public */,
      19,    0,   98,    2, 0x0a /* Public */,
      20,    1,   99,    2, 0x0a /* Public */,
      21,    1,  102,    2, 0x0a /* Public */,
      22,    1,  105,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    6,    7,    8,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Bool, QMetaType::Bool,   10,   12,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   14,   16,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, QMetaType::Int,   14,

       0        // eod
};

void MessageBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MessageBox *_t = static_cast<MessageBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Rename((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->reDownload((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 2: _t->Clearrecycle((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->Deletedownload((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->closeConfirm((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->unusualConfirm((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->onRenamelineeditChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onRenameSureBtnClicked(); break;
        case 8: _t->onClearBtnClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->onDeleteBtnClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->onExitBtnClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MessageBox::*)(QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageBox::Rename)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MessageBox::*)(const QString , QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageBox::reDownload)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MessageBox::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageBox::Clearrecycle)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MessageBox::*)(bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageBox::Deletedownload)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MessageBox::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageBox::closeConfirm)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MessageBox::*)(int , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageBox::unusualConfirm)) {
                *result = 5;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MessageBox::staticMetaObject = {
    { &DDialog::staticMetaObject, qt_meta_stringdata_MessageBox.data,
      qt_meta_data_MessageBox,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *MessageBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MessageBox::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MessageBox.stringdata0))
        return static_cast<void*>(this);
    return DDialog::qt_metacast(_clname);
}

int MessageBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void MessageBox::Rename(QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MessageBox::reDownload(const QString _t1, QString _t2, QString _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MessageBox::Clearrecycle(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MessageBox::Deletedownload(bool _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void MessageBox::closeConfirm(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MessageBox::unusualConfirm(int _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
