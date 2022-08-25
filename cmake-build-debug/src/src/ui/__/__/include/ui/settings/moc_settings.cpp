/****************************************************************************
** Meta object code from reading C++ file 'settings.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../../../src/include/ui/settings/settings.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'settings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Settings_t {
    QByteArrayData data[16];
    char stringdata0[276];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Settings_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Settings_t qt_meta_stringdata_Settings = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Settings"
QT_MOC_LITERAL(1, 9, 14), // "poweronChanged"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 5), // "state"
QT_MOC_LITERAL(4, 31, 28), // "maxDownloadTaskNumberChanged"
QT_MOC_LITERAL(5, 60, 10), // "taskNumber"
QT_MOC_LITERAL(6, 71, 10), // "isStopTask"
QT_MOC_LITERAL(7, 82, 8), // "isAddOne"
QT_MOC_LITERAL(8, 91, 23), // "downloadSettingsChanged"
QT_MOC_LITERAL(9, 115, 17), // "disckCacheChanged"
QT_MOC_LITERAL(10, 133, 6), // "number"
QT_MOC_LITERAL(11, 140, 28), // "startAssociatedBTFileChanged"
QT_MOC_LITERAL(12, 169, 21), // "controlBrowserChanged"
QT_MOC_LITERAL(13, 191, 34), // "startAssociatedMetaLinkFileCh..."
QT_MOC_LITERAL(14, 226, 26), // "autoDownloadBySpeedChanged"
QT_MOC_LITERAL(15, 253, 22) // "autoSortBySpeedChanged"

    },
    "Settings\0poweronChanged\0\0state\0"
    "maxDownloadTaskNumberChanged\0taskNumber\0"
    "isStopTask\0isAddOne\0downloadSettingsChanged\0"
    "disckCacheChanged\0number\0"
    "startAssociatedBTFileChanged\0"
    "controlBrowserChanged\0"
    "startAssociatedMetaLinkFileChanged\0"
    "autoDownloadBySpeedChanged\0"
    "autoSortBySpeedChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Settings[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    3,   62,    2, 0x06 /* Public */,
       8,    0,   69,    2, 0x06 /* Public */,
       9,    1,   70,    2, 0x06 /* Public */,
      11,    1,   73,    2, 0x06 /* Public */,
      12,    1,   76,    2, 0x06 /* Public */,
      13,    1,   79,    2, 0x06 /* Public */,
      14,    1,   82,    2, 0x06 /* Public */,
      15,    1,   85,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool, QMetaType::Bool,    5,    6,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,

       0        // eod
};

void Settings::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Settings *_t = static_cast<Settings *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->poweronChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->maxDownloadTaskNumberChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 2: _t->downloadSettingsChanged(); break;
        case 3: _t->disckCacheChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->startAssociatedBTFileChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->controlBrowserChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->startAssociatedMetaLinkFileChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->autoDownloadBySpeedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->autoSortBySpeedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Settings::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::poweronChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Settings::*)(int , bool , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::maxDownloadTaskNumberChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Settings::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::downloadSettingsChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Settings::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::disckCacheChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Settings::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::startAssociatedBTFileChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Settings::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::controlBrowserChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Settings::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::startAssociatedMetaLinkFileChanged)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Settings::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::autoDownloadBySpeedChanged)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (Settings::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Settings::autoSortBySpeedChanged)) {
                *result = 8;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Settings::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Settings.data,
      qt_meta_data_Settings,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Settings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Settings::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Settings.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Settings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void Settings::poweronChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Settings::maxDownloadTaskNumberChanged(int _t1, bool _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Settings::downloadSettingsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Settings::disckCacheChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Settings::startAssociatedBTFileChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Settings::controlBrowserChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Settings::startAssociatedMetaLinkFileChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Settings::autoDownloadBySpeedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void Settings::autoSortBySpeedChanged(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
