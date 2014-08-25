/****************************************************************************
** Meta object code from reading C++ file 'corefilter.h'
**
** Created: Mon Aug 25 10:29:57 2014
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "corefilter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'corefilter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CGraphQtWatcher[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_CGraphQtWatcher[] = {
    "CGraphQtWatcher\0\0progress(int)\0"
};

const QMetaObject CGraphQtWatcher::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CGraphQtWatcher,
      qt_meta_data_CGraphQtWatcher, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CGraphQtWatcher::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CGraphQtWatcher::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CGraphQtWatcher::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CGraphQtWatcher))
        return static_cast<void*>(const_cast< CGraphQtWatcher*>(this));
    if (!strcmp(_clname, "CGraphWatcher"))
        return static_cast< CGraphWatcher*>(const_cast< CGraphQtWatcher*>(this));
    return QObject::qt_metacast(_clname);
}

int CGraphQtWatcher::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: progress((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void CGraphQtWatcher::progress(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_CoreFilter[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_CoreFilter[] = {
    "CoreFilter\0"
};

const QMetaObject CoreFilter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CoreFilter,
      qt_meta_data_CoreFilter, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CoreFilter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CoreFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CoreFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CoreFilter))
        return static_cast<void*>(const_cast< CoreFilter*>(this));
    return QObject::qt_metacast(_clname);
}

int CoreFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
