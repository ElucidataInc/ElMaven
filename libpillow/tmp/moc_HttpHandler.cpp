/****************************************************************************
** Meta object code from reading C++ file 'HttpHandler.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../HttpHandler.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HttpHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Pillow__HttpHandler[] = {

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
      37,   26,   21,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpHandler[] = {
    "Pillow::HttpHandler\0\0bool\0connection\0"
    "handleRequest(Pillow::HttpConnection*)\0"
};

void Pillow::HttpHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HttpHandler *_t = static_cast<HttpHandler *>(_o);
        switch (_id) {
        case 0: { bool _r = _t->handleRequest((*reinterpret_cast< Pillow::HttpConnection*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Pillow::HttpHandler::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandler,
      qt_meta_data_Pillow__HttpHandler, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandler::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandler))
        return static_cast<void*>(const_cast< HttpHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int Pillow::HttpHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_Pillow__HttpHandlerStack[] = {

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

static const char qt_meta_stringdata_Pillow__HttpHandlerStack[] = {
    "Pillow::HttpHandlerStack\0"
};

void Pillow::HttpHandlerStack::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pillow::HttpHandlerStack::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandlerStack::staticMetaObject = {
    { &HttpHandler::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandlerStack,
      qt_meta_data_Pillow__HttpHandlerStack, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandlerStack::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandlerStack::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandlerStack::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandlerStack))
        return static_cast<void*>(const_cast< HttpHandlerStack*>(this));
    return HttpHandler::qt_metacast(_clname);
}

int Pillow::HttpHandlerStack::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = HttpHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Pillow__HttpHandlerFixed[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       2,   29, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   36,   25,   25, 0x0a,
      74,   66,   25,   25, 0x0a,

 // properties: name, type, flags
      36,   97, 0x02495103,
      66,  101, 0x0c495103,

 // properties: notify_signal_id
       0,
       0,

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpHandlerFixed[] = {
    "Pillow::HttpHandlerFixed\0\0changed()\0"
    "statusCode\0setStatusCode(int)\0content\0"
    "setContent(QByteArray)\0int\0QByteArray\0"
};

void Pillow::HttpHandlerFixed::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HttpHandlerFixed *_t = static_cast<HttpHandlerFixed *>(_o);
        switch (_id) {
        case 0: _t->changed(); break;
        case 1: _t->setStatusCode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setContent((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Pillow::HttpHandlerFixed::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandlerFixed::staticMetaObject = {
    { &HttpHandler::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandlerFixed,
      qt_meta_data_Pillow__HttpHandlerFixed, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandlerFixed::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandlerFixed::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandlerFixed::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandlerFixed))
        return static_cast<void*>(const_cast< HttpHandlerFixed*>(this));
    return HttpHandler::qt_metacast(_clname);
}

int Pillow::HttpHandlerFixed::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = HttpHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = statusCode(); break;
        case 1: *reinterpret_cast< QByteArray*>(_v) = content(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setStatusCode(*reinterpret_cast< int*>(_v)); break;
        case 1: setContent(*reinterpret_cast< QByteArray*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Pillow::HttpHandlerFixed::changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_Pillow__HttpHandler404[] = {

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

static const char qt_meta_stringdata_Pillow__HttpHandler404[] = {
    "Pillow::HttpHandler404\0"
};

void Pillow::HttpHandler404::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pillow::HttpHandler404::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandler404::staticMetaObject = {
    { &HttpHandler::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandler404,
      qt_meta_data_Pillow__HttpHandler404, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandler404::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandler404::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandler404::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandler404))
        return static_cast<void*>(const_cast< HttpHandler404*>(this));
    return HttpHandler::qt_metacast(_clname);
}

int Pillow::HttpHandler404::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = HttpHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Pillow__HttpHandlerFunction[] = {

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

static const char qt_meta_stringdata_Pillow__HttpHandlerFunction[] = {
    "Pillow::HttpHandlerFunction\0"
};

void Pillow::HttpHandlerFunction::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pillow::HttpHandlerFunction::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandlerFunction::staticMetaObject = {
    { &HttpHandler::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandlerFunction,
      qt_meta_data_Pillow__HttpHandlerFunction, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandlerFunction::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandlerFunction::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandlerFunction::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandlerFunction))
        return static_cast<void*>(const_cast< HttpHandlerFunction*>(this));
    return HttpHandler::qt_metacast(_clname);
}

int Pillow::HttpHandlerFunction::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = HttpHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Pillow__HttpHandlerLog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       1,   44, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      29,   24,   23,   23, 0x0a,
      50,   43,   23,   23, 0x0a,
      83,   72,   23,   23, 0x08,
     125,   72,   23,   23, 0x08,
     164,   72,   23,   23, 0x08,
     197,  191,   23,   23, 0x08,

 // enums: name, flags, count, data
     210, 0x0,    2,   48,

 // enum data: key, value
     215, uint(Pillow::HttpHandlerLog::LogCompletedRequests),
     236, uint(Pillow::HttpHandlerLog::TraceRequests),

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpHandlerLog[] = {
    "Pillow::HttpHandlerLog\0\0mode\0setMode(Mode)\0"
    "device\0setDevice(QIODevice*)\0connection\0"
    "requestCompleted(Pillow::HttpConnection*)\0"
    "requestClosed(Pillow::HttpConnection*)\0"
    "requestDestroyed(QObject*)\0entry\0"
    "log(QString)\0Mode\0LogCompletedRequests\0"
    "TraceRequests\0"
};

void Pillow::HttpHandlerLog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HttpHandlerLog *_t = static_cast<HttpHandlerLog *>(_o);
        switch (_id) {
        case 0: _t->setMode((*reinterpret_cast< Mode(*)>(_a[1]))); break;
        case 1: _t->setDevice((*reinterpret_cast< QIODevice*(*)>(_a[1]))); break;
        case 2: _t->requestCompleted((*reinterpret_cast< Pillow::HttpConnection*(*)>(_a[1]))); break;
        case 3: _t->requestClosed((*reinterpret_cast< Pillow::HttpConnection*(*)>(_a[1]))); break;
        case 4: _t->requestDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 5: _t->log((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Pillow::HttpHandlerLog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandlerLog::staticMetaObject = {
    { &HttpHandler::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandlerLog,
      qt_meta_data_Pillow__HttpHandlerLog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandlerLog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandlerLog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandlerLog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandlerLog))
        return static_cast<void*>(const_cast< HttpHandlerLog*>(this));
    return HttpHandler::qt_metacast(_clname);
}

int Pillow::HttpHandlerLog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = HttpHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
static const uint qt_meta_data_Pillow__HttpHandlerFile[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       1,   19, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      25,   24,   24,   24, 0x05,

 // properties: name, type, flags
      43,   35, 0x0a495103,

 // properties: notify_signal_id
       0,

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpHandlerFile[] = {
    "Pillow::HttpHandlerFile\0\0changed()\0"
    "QString\0publicPath\0"
};

void Pillow::HttpHandlerFile::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HttpHandlerFile *_t = static_cast<HttpHandlerFile *>(_o);
        switch (_id) {
        case 0: _t->changed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pillow::HttpHandlerFile::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandlerFile::staticMetaObject = {
    { &HttpHandler::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandlerFile,
      qt_meta_data_Pillow__HttpHandlerFile, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandlerFile::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandlerFile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandlerFile::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandlerFile))
        return static_cast<void*>(const_cast< HttpHandlerFile*>(this));
    return HttpHandler::qt_metacast(_clname);
}

int Pillow::HttpHandlerFile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = HttpHandler::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = publicPath(); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setPublicPath(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Pillow::HttpHandlerFile::changed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_Pillow__HttpHandlerFileTransfer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x05,

 // slots: signature, parameters, type, tag, flags
      44,   32,   32,   32, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Pillow__HttpHandlerFileTransfer[] = {
    "Pillow::HttpHandlerFileTransfer\0\0"
    "finished()\0writeNextPayload()\0"
};

void Pillow::HttpHandlerFileTransfer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        HttpHandlerFileTransfer *_t = static_cast<HttpHandlerFileTransfer *>(_o);
        switch (_id) {
        case 0: _t->finished(); break;
        case 1: _t->writeNextPayload(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pillow::HttpHandlerFileTransfer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandlerFileTransfer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandlerFileTransfer,
      qt_meta_data_Pillow__HttpHandlerFileTransfer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandlerFileTransfer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandlerFileTransfer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandlerFileTransfer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandlerFileTransfer))
        return static_cast<void*>(const_cast< HttpHandlerFileTransfer*>(this));
    return QObject::qt_metacast(_clname);
}

int Pillow::HttpHandlerFileTransfer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void Pillow::HttpHandlerFileTransfer::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
