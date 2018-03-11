/****************************************************************************
** Meta object code from reading C++ file 'yamlrpcserver.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../cyclops_server/yamlrpcserver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'yamlrpcserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_YamlRPCServer[] = {

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
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      22,   14,   14,   14, 0x09,
      35,   14,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_YamlRPCServer[] = {
    "YamlRPCServer\0\0quit()\0readClient()\0"
    "discardClient()\0"
};

void YamlRPCServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        YamlRPCServer *_t = static_cast<YamlRPCServer *>(_o);
        switch (_id) {
        case 0: _t->quit(); break;
        case 1: _t->readClient(); break;
        case 2: _t->discardClient(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData YamlRPCServer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject YamlRPCServer::staticMetaObject = {
    { &QTcpServer::staticMetaObject, qt_meta_stringdata_YamlRPCServer,
      qt_meta_data_YamlRPCServer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &YamlRPCServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *YamlRPCServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *YamlRPCServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_YamlRPCServer))
        return static_cast<void*>(const_cast< YamlRPCServer*>(this));
    return QTcpServer::qt_metacast(_clname);
}

int YamlRPCServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
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
void YamlRPCServer::quit()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
