/****************************************************************************
** Meta object code from reading C++ file 'hcalcolo_costi_item.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../hcalcolo_costi_item.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hcalcolo_costi_item.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_HCalcolo_costi_item_t {
    QByteArrayData data[10];
    char stringdata0[161];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HCalcolo_costi_item_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HCalcolo_costi_item_t qt_meta_stringdata_HCalcolo_costi_item = {
    {
QT_MOC_LITERAL(0, 0, 19), // "HCalcolo_costi_item"
QT_MOC_LITERAL(1, 20, 10), // "item_added"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 4), // "item"
QT_MOC_LITERAL(4, 37, 5), // "costo"
QT_MOC_LITERAL(5, 43, 25), // "on_le_costo_returnPressed"
QT_MOC_LITERAL(6, 69, 33), // "on_cbProdotti_currentIndexCha..."
QT_MOC_LITERAL(7, 103, 5), // "index"
QT_MOC_LITERAL(8, 109, 25), // "on_leAmount_returnPressed"
QT_MOC_LITERAL(9, 135, 25) // "on_leResult_returnPressed"

    },
    "HCalcolo_costi_item\0item_added\0\0item\0"
    "costo\0on_le_costo_returnPressed\0"
    "on_cbProdotti_currentIndexChanged\0"
    "index\0on_leAmount_returnPressed\0"
    "on_leResult_returnPressed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HCalcolo_costi_item[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   44,    2, 0x08 /* Private */,
       6,    1,   45,    2, 0x08 /* Private */,
       8,    0,   48,    2, 0x08 /* Private */,
       9,    0,   49,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Int, QMetaType::QString, QMetaType::QString,    3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void HCalcolo_costi_item::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<HCalcolo_costi_item *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { int _r = _t->item_added((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 1: _t->on_le_costo_returnPressed(); break;
        case 2: _t->on_cbProdotti_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_leAmount_returnPressed(); break;
        case 4: _t->on_leResult_returnPressed(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = int (HCalcolo_costi_item::*)(QString , QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&HCalcolo_costi_item::item_added)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject HCalcolo_costi_item::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_HCalcolo_costi_item.data,
    qt_meta_data_HCalcolo_costi_item,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HCalcolo_costi_item::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HCalcolo_costi_item::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HCalcolo_costi_item.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int HCalcolo_costi_item::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
int HCalcolo_costi_item::item_added(QString _t1, QString _t2)
{
    int _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
    return _t0;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
