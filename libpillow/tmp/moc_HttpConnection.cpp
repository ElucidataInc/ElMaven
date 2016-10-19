/****************************************************************************
** Meta object code from reading C++ file 'HttpConnection.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../HttpConnection.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HttpConnection.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Pillow__HttpConnection[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      26,   14, // methods
      12,  144, // properties
       1,  192, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      29,   24,   23,   23, 0x05,
      67,   24,   23,   23, 0x05,
     109,   24,   23,   23, 0x05,

 // slots: signature, parameters, type, tag, flags
     168,  141,   23,   23, 0x0a,
     246,  227,   23,   23, 0x2a,
     305,  294,   23,   23, 0x2a,
     324,   23,   23,   23, 0x2a,
     340,  141,   23,   23, 0x0a,
     402,  227,   23,   23, 0x2a,
     456,  294,   23,   23, 0x2a,
     481,   23,   23,   23, 0x2a,
     503,  227,   23,   23, 0x0a,
     550,  294,   23,   23, 0x2a,
     568,   23,   23,   23, 0x2a,
     591,  583,   23,   23, 0x0a,
     616,   23,   23,   23, 0x0a,
     629,   23,   23,   23, 0x0a,
     637,   23,   23,   23, 0x0a,
     649,   23,  645,   23, 0x0a,
     677,   23,  670,   23, 0x0a,
     701,   23,   23,   23, 0x08,
     716,   23,   23,   23, 0x08,

 // methods: signature, parameters, type, tag, flags
     724,   23,   23,   23, 0x02,
     747,  741,   23,   23, 0x02,
     791,  786,  778,   23, 0x02,
     829,  818,   23,   23, 0x02,

 // properties: name, type, flags
     868,  862, 0x00095009,
     885,  874, 0x0c495001,
     899,  874, 0x0c495001,
     910,  778, 0x0a495001,
     928,  874, 0x0c495001,
     940,  778, 0x0a495001,
     959,  874, 0x0c495001,
     978,  778, 0x0a495001,
    1004,  874, 0x0c495001,
    1020,  778, 0x0a495001,
    1043,  874, 0x0c495001,
    1062,  874, 0x0c495001,

 // properties: notify_signal_id
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,

 // enums: name, flags, count, data
     862, 0x0,    8,  196,

 // enum data: key, value
    1077, uint(Pillow::HttpConnection::Uninitialized),
    1091, uint(Pillow::HttpConnection::ReceivingHeaders),
    1108, uint(Pillow::HttpConnection::ReceivingContent),
    1125, uint(Pillow::HttpConnection::SendingHeaders),
    1140, uint(Pillow::HttpConnection::SendingContent),
    1155, uint(Pillow::HttpConnection::Completed),
    1165, uint(Pillow::HttpConnection::Flushing),
    1174, uint(Pillow::HttpConnection::Closed),

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpConnection[] = {
    "Pillow::HttpConnection\0\0self\0"
    "requestReady(Pillow::HttpConnection*)\0"
    "requestCompleted(Pillow::HttpConnection*)\0"
    "closed(Pillow::HttpConnection*)\0"
    "statusCode,headers,content\0"
    "writeResponse(int,Pillow::HttpHeaderCollection,QByteArray)\0"
    "statusCode,headers\0"
    "writeResponse(int,Pillow::HttpHeaderCollection)\0"
    "statusCode\0writeResponse(int)\0"
    "writeResponse()\0"
    "writeResponseString(int,Pillow::HttpHeaderCollection,QString)\0"
    "writeResponseString(int,Pillow::HttpHeaderCollection)\0"
    "writeResponseString(int)\0writeResponseString()\0"
    "writeHeaders(int,Pillow::HttpHeaderCollection)\0"
    "writeHeaders(int)\0writeHeaders()\0"
    "content\0writeContent(QByteArray)\0"
    "endContent()\0flush()\0close()\0int\0"
    "responseStatusCode()\0qint64\0"
    "responseContentLength()\0processInput()\0"
    "drain()\0requestHeaders()\0field\0"
    "requestHeaderValue(QByteArray)\0QString\0"
    "name\0requestParamValue(QString)\0"
    "name,value\0setRequestParam(QString,QString)\0"
    "State\0state\0QByteArray\0requestMethod\0"
    "requestUri\0requestUriDecoded\0requestPath\0"
    "requestPathDecoded\0requestQueryString\0"
    "requestQueryStringDecoded\0requestFragment\0"
    "requestFragmentDecoded\0requestHttpVersion\0"
    "requestContent\0Uninitialized\0"
    "ReceivingHeaders\0ReceivingContent\0"
    "SendingHeaders\0SendingContent\0Completed\0"
    "Flushing\0Closed\0"
};

void Pillow::HttpConnection::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HttpConnection *_t = static_cast<HttpConnection *>(_o);
        switch (_id) {
        case 0: _t->requestReady((*reinterpret_cast< Pillow::HttpConnection*(*)>(_a[1]))); break;
        case 1: _t->requestCompleted((*reinterpret_cast< Pillow::HttpConnection*(*)>(_a[1]))); break;
        case 2: _t->closed((*reinterpret_cast< Pillow::HttpConnection*(*)>(_a[1]))); break;
        case 3: _t->writeResponse((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const Pillow::HttpHeaderCollection(*)>(_a[2])),(*reinterpret_cast< const QByteArray(*)>(_a[3]))); break;
        case 4: _t->writeResponse((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const Pillow::HttpHeaderCollection(*)>(_a[2]))); break;
        case 5: _t->writeResponse((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->writeResponse(); break;
        case 7: _t->writeResponseString((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const Pillow::HttpHeaderCollection(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 8: _t->writeResponseString((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const Pillow::HttpHeaderCollection(*)>(_a[2]))); break;
        case 9: _t->writeResponseString((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->writeResponseString(); break;
        case 11: _t->writeHeaders((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const Pillow::HttpHeaderCollection(*)>(_a[2]))); break;
        case 12: _t->writeHeaders((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->writeHeaders(); break;
        case 14: _t->writeContent((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 15: _t->endContent(); break;
        case 16: _t->flush(); break;
        case 17: _t->close(); break;
        case 18: { int _r = _t->responseStatusCode();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 19: { qint64 _r = _t->responseContentLength();
            if (_a[0]) *reinterpret_cast< qint64*>(_a[0]) = _r; }  break;
        case 20: _t->processInput(); break;
        case 21: _t->drain(); break;
        case 22: _t->requestHeaders(); break;
        case 23: _t->requestHeaderValue((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 24: { QString _r = _t->requestParamValue((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 25: _t->setRequestParam((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Pillow::HttpConnection::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpConnection::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Pillow__HttpConnection,
      qt_meta_data_Pillow__HttpConnection, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpConnection::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpConnection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpConnection::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpConnection))
        return static_cast<void*>(const_cast< HttpConnection*>(this));
    return QObject::qt_metacast(_clname);
}

int Pillow::HttpConnection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 26)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 26;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< State*>(_v) = state(); break;
        case 1: *reinterpret_cast< QByteArray*>(_v) = requestMethod(); break;
        case 2: *reinterpret_cast< QByteArray*>(_v) = requestUri(); break;
        case 3: *reinterpret_cast< QString*>(_v) = requestUriDecoded(); break;
        case 4: *reinterpret_cast< QByteArray*>(_v) = requestPath(); break;
        case 5: *reinterpret_cast< QString*>(_v) = requestPathDecoded(); break;
        case 6: *reinterpret_cast< QByteArray*>(_v) = requestQueryString(); break;
        case 7: *reinterpret_cast< QString*>(_v) = requestQueryStringDecoded(); break;
        case 8: *reinterpret_cast< QByteArray*>(_v) = requestFragment(); break;
        case 9: *reinterpret_cast< QString*>(_v) = requestFragmentDecoded(); break;
        case 10: *reinterpret_cast< QByteArray*>(_v) = requestHttpVersion(); break;
        case 11: *reinterpret_cast< QByteArray*>(_v) = requestContent(); break;
        }
        _id -= 12;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 12;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 12;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 12;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Pillow::HttpConnection::requestReady(Pillow::HttpConnection * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Pillow::HttpConnection::requestCompleted(Pillow::HttpConnection * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Pillow::HttpConnection::closed(Pillow::HttpConnection * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
