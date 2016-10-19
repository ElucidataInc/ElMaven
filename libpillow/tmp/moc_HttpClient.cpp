/****************************************************************************
** Meta object code from reading C++ file 'HttpClient.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../HttpClient.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HttpClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Pillow__HttpClient[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       1,   44, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x05,
      39,   19,   19,   19, 0x05,
      58,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      75,   69,   19,   19, 0x08,
     118,   19,   19,   19, 0x08,
     137,   19,   19,   19, 0x08,

 // enums: name, flags, count, data
     156, 0x1,    5,   48,

 // enum data: key, value
     162, uint(Pillow::HttpClient::NoError),
     170, uint(Pillow::HttpClient::NetworkError),
     183, uint(Pillow::HttpClient::ResponseInvalidError),
     204, uint(Pillow::HttpClient::RemoteHostClosedError),
     226, uint(Pillow::HttpClient::AbortedError),

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpClient[] = {
    "Pillow::HttpClient\0\0headersCompleted()\0"
    "contentReadyRead()\0finished()\0error\0"
    "device_error(QAbstractSocket::SocketError)\0"
    "device_connected()\0device_readyRead()\0"
    "Error\0NoError\0NetworkError\0"
    "ResponseInvalidError\0RemoteHostClosedError\0"
    "AbortedError\0"
};

void Pillow::HttpClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HttpClient *_t = static_cast<HttpClient *>(_o);
        switch (_id) {
        case 0: _t->headersCompleted(); break;
        case 1: _t->contentReadyRead(); break;
        case 2: _t->finished(); break;
        case 3: _t->device_error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 4: _t->device_connected(); break;
        case 5: _t->device_readyRead(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Pillow::HttpClient::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Pillow__HttpClient,
      qt_meta_data_Pillow__HttpClient, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpClient::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpClient))
        return static_cast<void*>(const_cast< HttpClient*>(this));
    if (!strcmp(_clname, "HttpResponseParser"))
        return static_cast< HttpResponseParser*>(const_cast< HttpClient*>(this));
    return QObject::qt_metacast(_clname);
}

int Pillow::HttpClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Pillow::HttpClient::headersCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Pillow::HttpClient::contentReadyRead()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Pillow::HttpClient::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
static const uint qt_meta_data_Pillow__NetworkAccessManager[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      30,   29,   29,   29, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Pillow__NetworkAccessManager[] = {
    "Pillow::NetworkAccessManager\0\0"
    "client_finished()\0"
};

void Pillow::NetworkAccessManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NetworkAccessManager *_t = static_cast<NetworkAccessManager *>(_o);
        switch (_id) {
        case 0: _t->client_finished(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pillow::NetworkAccessManager::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::NetworkAccessManager::staticMetaObject = {
    { &QNetworkAccessManager::staticMetaObject, qt_meta_stringdata_Pillow__NetworkAccessManager,
      qt_meta_data_Pillow__NetworkAccessManager, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::NetworkAccessManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::NetworkAccessManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::NetworkAccessManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__NetworkAccessManager))
        return static_cast<void*>(const_cast< NetworkAccessManager*>(this));
    return QNetworkAccessManager::qt_metacast(_clname);
}

int Pillow::NetworkAccessManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QNetworkAccessManager::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
