/****************************************************************************
** Meta object code from reading C++ file 'HttpHandlerProxy.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../HttpHandlerProxy.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HttpHandlerProxy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Pillow__HttpHandlerProxy[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_Pillow__HttpHandlerProxy[] = {
    "Pillow::HttpHandlerProxy\0"
};

void Pillow::HttpHandlerProxy::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pillow::HttpHandlerProxy::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandlerProxy::staticMetaObject = {
    { &Pillow::HttpHandler::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandlerProxy,
      qt_meta_data_Pillow__HttpHandlerProxy, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandlerProxy::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandlerProxy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandlerProxy::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandlerProxy))
        return static_cast<void*>(const_cast< HttpHandlerProxy*>(this));
    typedef Pillow::HttpHandler QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Pillow::HttpHandlerProxy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Pillow::HttpHandler QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Pillow__HttpHandlerProxyPipe[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      30,   29,   29,   29, 0x09,
      41,   29,   29,   29, 0x09,
      60,   55,   29,   29, 0x09,
      77,   29,   29,   29, 0x08,
     102,   29,   29,   29, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpHandlerProxyPipe[] = {
    "Pillow::HttpHandlerProxyPipe\0\0teardown()\0"
    "sendHeaders()\0data\0pump(QByteArray)\0"
    "proxiedReply_readyRead()\0"
    "proxiedReply_finished()\0"
};

void Pillow::HttpHandlerProxyPipe::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HttpHandlerProxyPipe *_t = static_cast<HttpHandlerProxyPipe *>(_o);
        switch (_id) {
        case 0: _t->teardown(); break;
        case 1: _t->sendHeaders(); break;
        case 2: _t->pump((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 3: _t->proxiedReply_readyRead(); break;
        case 4: _t->proxiedReply_finished(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Pillow::HttpHandlerProxyPipe::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandlerProxyPipe::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandlerProxyPipe,
      qt_meta_data_Pillow__HttpHandlerProxyPipe, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandlerProxyPipe::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandlerProxyPipe::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandlerProxyPipe::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandlerProxyPipe))
        return static_cast<void*>(const_cast< HttpHandlerProxyPipe*>(this));
    return QObject::qt_metacast(_clname);
}

int Pillow::HttpHandlerProxyPipe::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_Pillow__ElasticNetworkAccessManager[] = {

 // content:
       6,       // revision
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

static const char qt_meta_stringdata_Pillow__ElasticNetworkAccessManager[] = {
    "Pillow::ElasticNetworkAccessManager\0"
};

void Pillow::ElasticNetworkAccessManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pillow::ElasticNetworkAccessManager::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::ElasticNetworkAccessManager::staticMetaObject = {
    { &QNetworkAccessManager::staticMetaObject, qt_meta_stringdata_Pillow__ElasticNetworkAccessManager,
      qt_meta_data_Pillow__ElasticNetworkAccessManager, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::ElasticNetworkAccessManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::ElasticNetworkAccessManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::ElasticNetworkAccessManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__ElasticNetworkAccessManager))
        return static_cast<void*>(const_cast< ElasticNetworkAccessManager*>(this));
    return QNetworkAccessManager::qt_metacast(_clname);
}

int Pillow::ElasticNetworkAccessManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QNetworkAccessManager::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
