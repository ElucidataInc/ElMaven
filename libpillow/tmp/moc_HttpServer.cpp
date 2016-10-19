/****************************************************************************
** Meta object code from reading C++ file 'HttpServer.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../HttpServer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HttpServer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Pillow__HttpServer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       3,   24, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      31,   20,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      77,   69,   19,   19, 0x08,

 // properties: name, type, flags
     133,  120, 0x00095009,
     151,  147, 0x02095001,
     167,  162, 0x01095001,

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpServer[] = {
    "Pillow::HttpServer\0\0connection\0"
    "requestReady(Pillow::HttpConnection*)\0"
    "request\0connection_closed(Pillow::HttpConnection*)\0"
    "QHostAddress\0serverAddress\0int\0"
    "serverPort\0bool\0listening\0"
};

void Pillow::HttpServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HttpServer *_t = static_cast<HttpServer *>(_o);
        switch (_id) {
        case 0: _t->requestReady((*reinterpret_cast< Pillow::HttpConnection*(*)>(_a[1]))); break;
        case 1: _t->connection_closed((*reinterpret_cast< Pillow::HttpConnection*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Pillow::HttpServer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpServer::staticMetaObject = {
    { &QTcpServer::staticMetaObject, qt_meta_stringdata_Pillow__HttpServer,
      qt_meta_data_Pillow__HttpServer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpServer))
        return static_cast<void*>(const_cast< HttpServer*>(this));
    return QTcpServer::qt_metacast(_clname);
}

int Pillow::HttpServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QHostAddress*>(_v) = serverAddress(); break;
        case 1: *reinterpret_cast< int*>(_v) = serverPort(); break;
        case 2: *reinterpret_cast< bool*>(_v) = isListening(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Pillow::HttpServer::requestReady(Pillow::HttpConnection * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_Pillow__HttpLocalServer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      36,   25,   24,   24, 0x05,

 // slots: signature, parameters, type, tag, flags
      74,   24,   24,   24, 0x08,
     103,   95,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpLocalServer[] = {
    "Pillow::HttpLocalServer\0\0connection\0"
    "requestReady(Pillow::HttpConnection*)\0"
    "this_newConnection()\0request\0"
    "connection_closed(Pillow::HttpConnection*)\0"
};

void Pillow::HttpLocalServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HttpLocalServer *_t = static_cast<HttpLocalServer *>(_o);
        switch (_id) {
        case 0: _t->requestReady((*reinterpret_cast< Pillow::HttpConnection*(*)>(_a[1]))); break;
        case 1: _t->this_newConnection(); break;
        case 2: _t->connection_closed((*reinterpret_cast< Pillow::HttpConnection*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Pillow::HttpLocalServer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpLocalServer::staticMetaObject = {
    { &QLocalServer::staticMetaObject, qt_meta_stringdata_Pillow__HttpLocalServer,
      qt_meta_data_Pillow__HttpLocalServer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpLocalServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpLocalServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpLocalServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpLocalServer))
        return static_cast<void*>(const_cast< HttpLocalServer*>(this));
    return QLocalServer::qt_metacast(_clname);
}

int Pillow::HttpLocalServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLocalServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Pillow::HttpLocalServer::requestReady(Pillow::HttpConnection * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
