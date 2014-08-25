/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Mon Aug 25 10:29:56 2014
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,
      44,   11,   11,   11, 0x08,
      83,   81,   11,   11, 0x08,
     109,  103,   11,   11, 0x08,
     137,   11,   11,   11, 0x08,
     155,   11,   11,   11, 0x08,
     168,   11,   11,   11, 0x08,
     194,   11,   11,   11, 0x08,
     229,   11,   11,   11, 0x08,
     265,  103,   11,   11, 0x08,
     299,  294,   11,   11, 0x08,
     338,  294,   11,   11, 0x08,
     371,   11,   11,   11, 0x08,
     404,  103,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0on_actionLoad_Image_triggered()\0"
    "on_actionSynthetic_Image_triggered()\0"
    "s\0onStateChanged(int)\0value\0"
    "onProgressValueChanged(int)\0"
    "onGraphComputed()\0onFinished()\0"
    "on_pushButton_2_clicked()\0"
    "on_actionImpulse_noise_triggered()\0"
    "on_actionGaussian_noise_triggered()\0"
    "onFilteringValueChanged(int)\0arg1\0"
    "on_checkBoxComponent_stateChanged(int)\0"
    "on_checkBoxHSV_stateChanged(int)\0"
    "on_actionSave_result_triggered()\0"
    "onAdaptiveValueChanged(int)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_actionLoad_Image_triggered(); break;
        case 1: on_actionSynthetic_Image_triggered(); break;
        case 2: onStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: onProgressValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: onGraphComputed(); break;
        case 5: onFinished(); break;
        case 6: on_pushButton_2_clicked(); break;
        case 7: on_actionImpulse_noise_triggered(); break;
        case 8: on_actionGaussian_noise_triggered(); break;
        case 9: onFilteringValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: on_checkBoxComponent_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: on_checkBoxHSV_stateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: on_actionSave_result_triggered(); break;
        case 13: onAdaptiveValueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
