/****************************************************************************
** Meta object code from reading C++ file 'HttpsServer.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../HttpsServer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HttpsServer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Pillow__HttpsServer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x0a,
      53,   43,   20,   20, 0x0a,
     103,   91,   20,   20, 0x0a,
     146,  135,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpsServer[] = {
    "Pillow::HttpsServer\0\0sslSocket_encrypted()\0"
    "sslErrors\0sslSocket_sslErrors(QList<QSslError>)\0"
    "certificate\0setCertificate(QSslCertificate)\0"
    "privateKey\0setPrivateKey(QSslKey)\0"
};

void Pillow::HttpsServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HttpsServer *_t = static_cast<HttpsServer *>(_o);
        switch (_id) {
        case 0: _t->sslSocket_encrypted(); break;
        case 1: _t->sslSocket_sslErrors((*reinterpret_cast< const QList<QSslError>(*)>(_a[1]))); break;
        case 2: _t->setCertificate((*reinterpret_cast< const QSslCertificate(*)>(_a[1]))); break;
        case 3: _t->setPrivateKey((*reinterpret_cast< const QSslKey(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Pillow::HttpsServer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpsServer::staticMetaObject = {
    { &Pillow::HttpServer::staticMetaObject, qt_meta_stringdata_Pillow__HttpsServer,
      qt_meta_data_Pillow__HttpsServer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpsServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpsServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpsServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpsServer))
        return static_cast<void*>(const_cast< HttpsServer*>(this));
    typedef Pillow::HttpServer QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Pillow::HttpsServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Pillow::HttpServer QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
