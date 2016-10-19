/****************************************************************************
** Meta object code from reading C++ file 'HttpHandlerSimpleRouter.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../HttpHandlerSimpleRouter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HttpHandlerSimpleRouter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Pillow__HttpHandlerSimpleRouter[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       1,   14, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // enums: name, flags, count, data
      32, 0x0,    2,   18,

 // enum data: key, value
      51, uint(Pillow::HttpHandlerSimpleRouter::Return4xxResponse),
      69, uint(Pillow::HttpHandlerSimpleRouter::Passthrough),

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpHandlerSimpleRouter[] = {
    "Pillow::HttpHandlerSimpleRouter\0"
    "RoutingErrorAction\0Return4xxResponse\0"
    "Passthrough\0"
};

void Pillow::HttpHandlerSimpleRouter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pillow::HttpHandlerSimpleRouter::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandlerSimpleRouter::staticMetaObject = {
    { &Pillow::HttpHandler::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandlerSimpleRouter,
      qt_meta_data_Pillow__HttpHandlerSimpleRouter, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandlerSimpleRouter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandlerSimpleRouter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandlerSimpleRouter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandlerSimpleRouter))
        return static_cast<void*>(const_cast< HttpHandlerSimpleRouter*>(this));
    typedef Pillow::HttpHandler QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Pillow::HttpHandlerSimpleRouter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Pillow::HttpHandler QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
