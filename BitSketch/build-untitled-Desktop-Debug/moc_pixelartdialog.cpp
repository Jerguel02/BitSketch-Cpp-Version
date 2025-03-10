/****************************************************************************
** Meta object code from reading C++ file 'pixelartdialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../BitSketch/pixelartdialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pixelartdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PixelArtDialog_t {
    QByteArrayData data[12];
    char stringdata0[112];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PixelArtDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PixelArtDialog_t qt_meta_stringdata_PixelArtDialog = {
    {
QT_MOC_LITERAL(0, 0, 14), // "PixelArtDialog"
QT_MOC_LITERAL(1, 15, 9), // "applySize"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 9), // "openImage"
QT_MOC_LITERAL(4, 36, 11), // "chooseColor"
QT_MOC_LITERAL(5, 48, 4), // "undo"
QT_MOC_LITERAL(6, 53, 4), // "redo"
QT_MOC_LITERAL(7, 58, 6), // "zoomIn"
QT_MOC_LITERAL(8, 65, 9), // "zoomPoint"
QT_MOC_LITERAL(9, 75, 7), // "zoomOut"
QT_MOC_LITERAL(10, 83, 15), // "savePixelDesign"
QT_MOC_LITERAL(11, 99, 12) // "previewImage"

    },
    "PixelArtDialog\0applySize\0\0openImage\0"
    "chooseColor\0undo\0redo\0zoomIn\0zoomPoint\0"
    "zoomOut\0savePixelDesign\0previewImage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PixelArtDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x08 /* Private */,
       3,    0,   70,    2, 0x08 /* Private */,
       4,    0,   71,    2, 0x08 /* Private */,
       5,    0,   72,    2, 0x08 /* Private */,
       6,    0,   73,    2, 0x08 /* Private */,
       7,    1,   74,    2, 0x08 /* Private */,
       7,    0,   77,    2, 0x28 /* Private | MethodCloned */,
       9,    1,   78,    2, 0x08 /* Private */,
       9,    0,   81,    2, 0x28 /* Private | MethodCloned */,
      10,    0,   82,    2, 0x08 /* Private */,
      11,    0,   83,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PixelArtDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PixelArtDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->applySize(); break;
        case 1: _t->openImage(); break;
        case 2: _t->chooseColor(); break;
        case 3: _t->undo(); break;
        case 4: _t->redo(); break;
        case 5: _t->zoomIn((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 6: _t->zoomIn(); break;
        case 7: _t->zoomOut((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 8: _t->zoomOut(); break;
        case 9: _t->savePixelDesign(); break;
        case 10: _t->previewImage(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PixelArtDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_PixelArtDialog.data,
    qt_meta_data_PixelArtDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PixelArtDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PixelArtDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PixelArtDialog.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int PixelArtDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
