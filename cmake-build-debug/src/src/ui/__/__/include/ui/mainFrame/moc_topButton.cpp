/****************************************************************************
** Meta object code from reading C++ file 'topButton.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../../../src/include/ui/mainFrame/topButton.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'topButton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SearchWidget_t {
    QByteArrayData data[4];
    char stringdata0[33];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SearchWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SearchWidget_t qt_meta_stringdata_SearchWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "SearchWidget"
QT_MOC_LITERAL(1, 13, 10), // "keyPressed"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 7) // "Qt::Key"

    },
    "SearchWidget\0keyPressed\0\0Qt::Key"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SearchWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

       0        // eod
};

void SearchWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SearchWidget *_t = static_cast<SearchWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->keyPressed((*reinterpret_cast< Qt::Key(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SearchWidget::*)(Qt::Key );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SearchWidget::keyPressed)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SearchWidget::staticMetaObject = {
    { &Dtk::Widget::DSearchEdit::staticMetaObject, qt_meta_stringdata_SearchWidget.data,
      qt_meta_data_SearchWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *SearchWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SearchWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SearchWidget.stringdata0))
        return static_cast<void*>(this);
    return Dtk::Widget::DSearchEdit::qt_metacast(_clname);
}

int SearchWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Dtk::Widget::DSearchEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void SearchWidget::keyPressed(Qt::Key _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_TopButton_t {
    QByteArrayData data[14];
    char stringdata0[204];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TopButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TopButton_t qt_meta_stringdata_TopButton = {
    {
QT_MOC_LITERAL(0, 0, 9), // "TopButton"
QT_MOC_LITERAL(1, 10, 21), // "newDownloadBtnClicked"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 23), // "pauseDownloadBtnClicked"
QT_MOC_LITERAL(4, 57, 23), // "startDownloadBtnClicked"
QT_MOC_LITERAL(5, 81, 24), // "deleteDownloadBtnClicked"
QT_MOC_LITERAL(6, 106, 15), // "SearchEditFocus"
QT_MOC_LITERAL(7, 122, 5), // "focus"
QT_MOC_LITERAL(8, 128, 20), // "SearchEditTextChange"
QT_MOC_LITERAL(9, 149, 4), // "text"
QT_MOC_LITERAL(10, 154, 20), // "SearchEditKeyPressed"
QT_MOC_LITERAL(11, 175, 7), // "Qt::Key"
QT_MOC_LITERAL(12, 183, 14), // "onTableChanged"
QT_MOC_LITERAL(13, 198, 5) // "index"

    },
    "TopButton\0newDownloadBtnClicked\0\0"
    "pauseDownloadBtnClicked\0startDownloadBtnClicked\0"
    "deleteDownloadBtnClicked\0SearchEditFocus\0"
    "focus\0SearchEditTextChange\0text\0"
    "SearchEditKeyPressed\0Qt::Key\0"
    "onTableChanged\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TopButton[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    0,   56,    2, 0x06 /* Public */,
       5,    0,   57,    2, 0x06 /* Public */,
       6,    1,   58,    2, 0x06 /* Public */,
       8,    1,   61,    2, 0x06 /* Public */,
      10,    1,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    1,   67,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, 0x80000000 | 11,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,   13,

       0        // eod
};

void TopButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TopButton *_t = static_cast<TopButton *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newDownloadBtnClicked(); break;
        case 1: _t->pauseDownloadBtnClicked(); break;
        case 2: _t->startDownloadBtnClicked(); break;
        case 3: _t->deleteDownloadBtnClicked(); break;
        case 4: _t->SearchEditFocus((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->SearchEditTextChange((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->SearchEditKeyPressed((*reinterpret_cast< Qt::Key(*)>(_a[1]))); break;
        case 7: _t->onTableChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TopButton::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopButton::newDownloadBtnClicked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TopButton::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopButton::pauseDownloadBtnClicked)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TopButton::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopButton::startDownloadBtnClicked)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TopButton::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopButton::deleteDownloadBtnClicked)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (TopButton::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopButton::SearchEditFocus)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (TopButton::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopButton::SearchEditTextChange)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (TopButton::*)(Qt::Key );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TopButton::SearchEditKeyPressed)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TopButton::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TopButton.data,
      qt_meta_data_TopButton,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *TopButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TopButton::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TopButton.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TopButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void TopButton::newDownloadBtnClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void TopButton::pauseDownloadBtnClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void TopButton::startDownloadBtnClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void TopButton::deleteDownloadBtnClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void TopButton::SearchEditFocus(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void TopButton::SearchEditTextChange(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void TopButton::SearchEditKeyPressed(Qt::Key _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
