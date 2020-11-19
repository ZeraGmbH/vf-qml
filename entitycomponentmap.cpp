#include "entitycomponentmap.h"

#include <ve_commandevent.h>
#include <vcmp_componentdata.h>
#include <vcmp_remoteproceduredata.h>

#include <QString>
#include <QQmlEngine>
#include <QJSValueList>


using namespace VeinEvent;
using namespace VeinComponent;

Q_LOGGING_CATEGORY(VEIN_API_QML_INTROSPECTION, VEIN_DEBUGNAME_QML_INTROSPECTION)

namespace VeinApiQml
{
EntityComponentMap::EntityComponentMap(int t_entityId, const QVariantHash &t_entityIntrospection, QObject *t_parent) :
    QQmlPropertyMap(this, t_parent),
    m_entityIntrospection(t_entityIntrospection),
    m_entityId(t_entityId)
{
    Q_ASSERT(m_entityId>=0);
}

void EntityComponentMap::processComponentData(ComponentData *t_cData)
{
    switch(t_cData->eventCommand()) {
    case ComponentData::Command::CCMD_ADD:
    case ComponentData::Command::CCMD_SET:
        vCDebug(VEIN_API_QML_VERBOSE) << "Updated value" << t_cData->componentName() << t_cData->newValue();
        insert(t_cData->componentName(), t_cData->newValue()); // bypasses the function updateValue(...)
        break;
    case ComponentData::Command::CCMD_REMOVE:
        /// @note It is not possible to remove keys from the map; once a key has been added, you can only modify or clear its associated value.
        /// @note Keys that have been cleared will still appear in this list, even though their associated values are invalid
        clear(t_cData->componentName());
        break;
    case ComponentData::Command::CCMD_FETCH:
        if(m_pendingValues.contains(t_cData->componentName())) {
            vCDebug(VEIN_API_QML_VERBOSE) << "Fetched value" << t_cData->componentName() << t_cData->newValue();
            insert(t_cData->componentName(), t_cData->newValue()); // bypasses the function updateValue(...)
            m_pendingValues.removeAll(t_cData->componentName());
            if(m_pendingValues.isEmpty()) {
                emit sigEntityComplete(m_entityId);
            }
        }
        break;
    default:
        break;
    }
}

void EntityComponentMap::processRemoteProcedureData(RemoteProcedureData *t_rpcData)
{
    Q_ASSERT(t_rpcData != nullptr);

    const QUuid rpcIdentifier = t_rpcData->invokationData().value(RemoteProcedureData::s_callIdString).toUuid();
    if(m_pendingRPCCallbacks.contains(rpcIdentifier)) {
        switch(t_rpcData->command()) {
        case RemoteProcedureData::Command::RPCMD_RESULT:
            vCDebug(VEIN_API_QML_VERBOSE) << "Received RPC result for entity:" << m_entityId << "procedureName:" << t_rpcData->procedureName() << "rpcData:" << t_rpcData->invokationData();
            emit sigRPCFinished(rpcIdentifier, t_rpcData->invokationData());
            m_pendingRPCCallbacks.remove(rpcIdentifier);
            break;
        case RemoteProcedureData::Command::RPCMD_PROGRESS:
            emit sigRPCProgress(rpcIdentifier, t_rpcData->invokationData());
            break;
        default:
            break;
        }
    }
}

EntityComponentMap::DataState EntityComponentMap::state() const
{
    return m_state;
}

void EntityComponentMap::setState(EntityComponentMap::DataState t_dataState)
{
    m_state = t_dataState;
    if(m_state == DataState::ECM_PENDING) {
        loadEntityData();
    }
    emit sigStateChanged(m_state);
}

int EntityComponentMap::entityId() const
{
    return m_entityId;
}

bool EntityComponentMap::hasComponent(const QString &t_componentName) const
{
    return contains(t_componentName);
}

int EntityComponentMap::propertyCount() const
{
    return count();
}

QUuid EntityComponentMap::invokeRPC(const QString &t_procedureName, const QVariantMap &t_parameters)
{
    QUuid rpcIdentifier;
    //check if a component exists that is callable
    if(m_registeredRemoteProcedures.contains(t_procedureName)) {
        do {
            rpcIdentifier = QUuid::createUuid();
        } while(m_pendingRPCCallbacks.contains(rpcIdentifier)); //should only run once
        m_pendingRPCCallbacks.insert(rpcIdentifier, t_procedureName);
        QVariantMap rpcParamData;
        rpcParamData.insert(RemoteProcedureData::s_callIdString, rpcIdentifier);
        rpcParamData.insert(RemoteProcedureData::s_parameterString, t_parameters);

        RemoteProcedureData *rpcData = new RemoteProcedureData();
        rpcData->setEntityId(m_entityId);
        rpcData->setCommand(RemoteProcedureData::Command::RPCMD_CALL);
        rpcData->setEventOrigin(ComponentData::EventOrigin::EO_LOCAL);
        rpcData->setEventTarget(ComponentData::EventTarget::ET_ALL);
        rpcData->setProcedureName(t_procedureName);
        rpcData->setInvokationData(rpcParamData);
        CommandEvent *cEvent = new CommandEvent(CommandEvent::EventSubtype::TRANSACTION, rpcData);
        vCDebug(VEIN_API_QML_VERBOSE) << "Sending RPC to entity:" << m_entityId << "procedure:" << t_procedureName << "data:" << rpcParamData << "event:" << cEvent;

        emit sigSendEvent(cEvent);
    }
    else {
        if(!m_registeredRemoteProcedures.isEmpty()) {
            qWarning() << "No rpc with procedure name:" << t_procedureName;
            qWarning() << "Available:";
            for(const auto &procedure : m_registeredRemoteProcedures) {
                qWarning() << procedure;
            }
        }
        else {
            qWarning() << "No rpc procedures found";
        }
    }
    return rpcIdentifier;
}

void EntityComponentMap::cancelRPCInvokation(QUuid t_identifier)
{
    if(m_pendingRPCCallbacks.contains(t_identifier)) {
        QVariantMap rpcParamData;
        rpcParamData.insert(RemoteProcedureData::s_callIdString, t_identifier);

        RemoteProcedureData *rpcData = new RemoteProcedureData();
        rpcData->setEntityId(m_entityId);
        rpcData->setCommand(RemoteProcedureData::Command::RPCMD_CALL);
        rpcData->setEventOrigin(ComponentData::EventOrigin::EO_LOCAL);
        rpcData->setEventTarget(ComponentData::EventTarget::ET_ALL);
        rpcData->setProcedureName(m_pendingRPCCallbacks.value(t_identifier));
        rpcData->setInvokationData(rpcParamData);
        CommandEvent *cEvent = new CommandEvent(CommandEvent::EventSubtype::TRANSACTION, rpcData);
        vCDebug(VEIN_API_QML_VERBOSE) << "Sending RPC cancellation to entity:" << m_entityId << "procedure:" << rpcData->procedureName() << "data:" << rpcParamData << "event:" << cEvent;

        emit sigSendEvent(cEvent);
    }
}

QList<QString> EntityComponentMap::getRemoteProcedureList() const
{
    return m_registeredRemoteProcedures;
}

QVariant EntityComponentMap::updateValue(const QString &t_key, const QVariant &t_newValue)
{
    const QVariant retVal = value(t_key);
    if(Q_UNLIKELY(t_newValue.isValid() == false)) {
        vCDebug(VEIN_API_QML) << QString("Invalid value for entity: %1 component: %2 value: ").arg(m_entityId).arg(t_key) << t_newValue;
        VF_ASSERT(t_newValue.isValid(), "Invalid value set from QML");
    }
    else if(retVal != t_newValue) {
        ComponentData *cData = nullptr;
        CommandEvent *cEvent = nullptr;

        cData = new ComponentData();
        cData->setEntityId(m_entityId);
        cData->setCommand(ComponentData::Command::CCMD_SET);
        cData->setEventOrigin(ComponentData::EventOrigin::EO_LOCAL);
        cData->setEventTarget(ComponentData::EventTarget::ET_ALL);
        cData->setComponentName(t_key);

        if(Q_UNLIKELY(t_newValue.canConvert(QMetaType::QVariantList) && t_newValue.toList().isEmpty() == false)) {
            cData->setNewValue(t_newValue.toList());
        }
        else if(Q_UNLIKELY(t_newValue.canConvert(QMetaType::QVariantMap))) {
            cData->setNewValue(t_newValue.toMap());
        }
        else {
            cData->setNewValue(t_newValue);
        }

        cData->setOldValue(retVal);
        cEvent = new CommandEvent(CommandEvent::EventSubtype::TRANSACTION, cData);

        emit sigSendEvent(cEvent);
    }
    return retVal;
}

void EntityComponentMap::loadEntityData()
{
    CommandEvent *cEvent = nullptr;
    ComponentData *cData = nullptr;

    const QList<QString> tmpComponentList = m_entityIntrospection.value(QString("components")).toStringList();
    m_registeredRemoteProcedures = m_entityIntrospection.value(QString("procedures")).toStringList();
    m_pendingValues.append(tmpComponentList);
    for(const QString &tmpKey : tmpComponentList) {
        insert(tmpKey, QVariant()); // bypasses the function updateValue(...)

        cData = new ComponentData();
        cData->setEntityId(m_entityId);
        cData->setCommand(ComponentData::Command::CCMD_FETCH);
        cData->setEventOrigin(ComponentData::EventOrigin::EO_LOCAL);
        cData->setEventTarget(ComponentData::EventTarget::ET_ALL);
        cData->setComponentName(tmpKey);
        cEvent = new CommandEvent(CommandEvent::EventSubtype::TRANSACTION, cData);
        vCDebug(VEIN_API_QML_VERBOSE) << "Fetching entity data for entityId:" << m_entityId << "component:" << tmpKey << "event:" << cEvent;

        emit sigSendEvent(cEvent);
    }
}
} // end namespace VeinApiQml
