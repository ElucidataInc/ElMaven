/****************************************************************************
** Meta object code from reading C++ file 'HttpHandlerQtScript.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../HttpHandlerQtScript.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HttpHandlerQtScript.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Pillow__HttpHandlerQtScript[] = {

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

static const char qt_meta_stringdata_Pillow__HttpHandlerQtScript[] = {
    "Pillow::HttpHandlerQtScript\0"
};

void Pillow::HttpHandlerQtScript::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pillow::HttpHandlerQtScript::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandlerQtScript::staticMetaObject = {
    { &Pillow::HttpHandler::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandlerQtScript,
      qt_meta_data_Pillow__HttpHandlerQtScript, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandlerQtScript::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandlerQtScript::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandlerQtScript::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandlerQtScript))
        return static_cast<void*>(const_cast< HttpHandlerQtScript*>(this));
    typedef Pillow::HttpHandler QMocSuperClass;
    return QMocSuperClass::qt_metacast(_clname);
}

int Pillow::HttpHandlerQtScript::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    typedef Pillow::HttpHandler QMocSuperClass;
    _id = QMocSuperClass::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Pillow__HttpHandlerQtScriptFile[] = {

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

static const char qt_meta_stringdata_Pillow__HttpHandlerQtScriptFile[] = {
    "Pillow::HttpHandlerQtScriptFile\0"
};

void Pillow::HttpHandlerQtScriptFile::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Pillow::HttpHandlerQtScriptFile::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Pillow::HttpHandlerQtScriptFile::staticMetaObject = {
    { &HttpHandlerQtScript::staticMetaObject, qt_meta_stringdata_Pillow__HttpHandlerQtScriptFile,
      qt_meta_data_Pillow__HttpHandlerQtScriptFile, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Pillow::HttpHandlerQtScriptFile::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Pillow::HttpHandlerQtScriptFile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Pillow::HttpHandlerQtScriptFile::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Pillow__HttpHandlerQtScriptFile))
        return static_cast<void*>(const_cast< HttpHandlerQtScriptFile*>(this));
    return HttpHandlerQtScript::qt_metacast(_clname);
}

int Pillow::HttpHandlerQtScriptFile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = HttpHandlerQtScript::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
