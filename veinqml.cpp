#include "veinqml.h"

#include "entitycomponentmap.h"

#include <ve_commandevent.h>
#include <ve_eventdata.h>

#include <vcmp_componentdata.h>
#include <vcmp_entitydata.h>
#include <vcmp_errordata.h>
#include <vcmp_introspectiondata.h>

Q_LOGGING_CATEGORY(VEIN_API_QML, "\e[1;37m<Vein.Api.QML>\033[0m")
Q_LOGGING_CATEGORY(VEIN_API_QML_VERBOSE, "\e[0;37m<Vein.Api.QML>\033[0m")

using namespace VeinEvent;
using namespace VeinComponent;

namespace VeinApiQml
{

  VeinQml::VeinQml(QObject *t_parent) : EventSystem(t_parent)
  {

  }

  VeinQml::ConnectionState VeinQml::state() const
  {
    return m_state;
  }

  EntityComponentMap *VeinQml::getEntity(const QString &t_entityName) const
  {
    EntityComponentMap *retVal = 0;
    int entityId = idFromEntityName(t_entityName);

    if(entityId>=0 && m_entities.contains(entityId))
    {
      retVal = m_entities.value(entityId);
    }
    else
    {
      qCWarning(VEIN_API_QML) << "No entity found with name:" << t_entityName;
    }

    return retVal;
  }

  bool VeinQml::hasEntity(const QString &t_entityName) const
  {
    bool retVal = false;
    int entityId = idFromEntityName(t_entityName);

    if(entityId>=0 && m_entities.contains(entityId))
    {
      retVal = true;
    }
    return retVal;
  }



  VeinQml *VeinQml::getStaticInstance()
  {
    return m_staticInstance;
  }

  void VeinQml::setStaticInstance(VeinQml *t_instance)
  {
    if(t_instance)
    {
      m_staticInstance = t_instance;
    }
  }

  void VeinQml::setRequiredIds(QList<int> t_requiredEntityIds)
  {
    m_requiredIds = t_requiredEntityIds; /// @todo maybe only append the new ids? send unsubscribe events for removed ids... etc.
    vCDebug(VEIN_API_QML) << "Set required ids to:" << t_requiredEntityIds;
    foreach(int newId, m_requiredIds) /// @todo currently it's possible to send subscription events for entities already subscribed to
    {
      EntityData *eData = new EntityData();
      eData->setCommand(EntityData::Command::ECMD_SUBSCRIBE);
      eData->setEntityId(newId);
      eData->setEventOrigin(EntityData::EventOrigin::EO_LOCAL);
      eData->setEventTarget(EntityData::EventTarget::ET_ALL);

      CommandEvent *cEvent = new CommandEvent(CommandEvent::EventSubtype::TRANSACTION, eData);

      emit sigSendEvent(cEvent);
    }
  }

  bool VeinQml::processEvent(QEvent *t_event)
  {
    bool retVal = false;

    if(t_event->type()==CommandEvent::eventType())
    {
      CommandEvent *cEvent = 0;
      cEvent = static_cast<CommandEvent *>(t_event);

      if(cEvent != 0 && cEvent->eventSubtype() == CommandEvent::EventSubtype::NOTIFICATION)
      {
        vCDebug(VEIN_API_QML_VERBOSE) << "Processing command event:" << cEvent << cEvent->eventData()->type();

        /// @todo add support for network events (connected / disconnected / error)
        switch (cEvent->eventData()->type())
        {
          case ComponentData::dataType():
          {
            ComponentData *cData=0;
            cData = static_cast<ComponentData *>(cEvent->eventData());
            retVal = true;

            if(m_entities.contains(cData->entityId())) ///< @note component data is only processed after the introspection has been processed
            {
              m_entities.value(cData->entityId())->processComponentData(cData);
            }
            break;
          }
          case EntityData::dataType():
          {
            EntityData *eData=0;
            eData = static_cast<EntityData *>(cEvent->eventData());
            retVal = true;
            int entityId =eData->entityId();

            switch(eData->eventCommand())
            {
              case VeinComponent::EntityData::ECMD_REMOVE:
              {
                if(m_entities.contains(entityId))
                {
                  /// @note do not delete the value here, as QML is still referencing it, instead mark it as removed
                  EntityComponentMap *eMap = m_entities.value(entityId);
                  eMap->setState(EntityComponentMap::DataState::ECM_REMOVED);
                  m_entities.remove(entityId);

                  if(m_requiredIds.contains(entityId))
                  {
                    qCCritical(VEIN_API_QML_INTROSPECTION) << "Required entity was removed remotely, entity id:" << entityId;
                    m_state = ConnectionState::VQ_ERROR;
                    emit sigStateChanged(m_state);
                  }
                }
                break;
              }
              default:
                break;
            }
            break;
          }
          case ErrorData::dataType(): ///< @todo add message queue and check if the error belongs to actions taken from this client
          {
            ErrorData *errData=0;
            errData = static_cast<ErrorData *>(cEvent->eventData());
            qCWarning(VEIN_API_QML_INTROSPECTION) << "Received error:" <<errData->errorDescription();
            break;
          }
          case IntrospectionData::dataType():
          {
            IntrospectionData *iData=0;
            iData = static_cast<IntrospectionData *>(cEvent->eventData());
            retVal = true;
            int entityId = iData->entityId();
            vCDebug(VEIN_API_QML) << "Received introspection data for entity:" << entityId;

            if(m_entities.contains(entityId) == false)
            {
              vCDebug(VEIN_API_QML) << "added introspection for entity:" << entityId;
              EntityComponentMap *eMap = new EntityComponentMap(entityId, iData->jsonData(), this);
              m_entities.insert(entityId, eMap);
              connect(eMap, &EntityComponentMap::sigSendEvent, this, &VeinQml::sigSendEvent);
              connect(eMap, &EntityComponentMap::sigLoadedChanged, this, &VeinQml::onEntityLoaded);
              eMap->setState(EntityComponentMap::DataState::ECM_PENDING);
            }
            break;
          }
          default:
            break;
        }
      }
    }
    return retVal;
  }

  void VeinQml::onEntityLoaded(int t_entityId)
  {
    if(m_requiredIds.contains(t_entityId))
    {
      vCDebug(VEIN_API_QML) << "Fetched required entity:" << t_entityId;
      m_requiredIds.removeAll(t_entityId);
      if(m_state != ConnectionState::VQ_LOADED)
      {
        if(m_requiredIds.isEmpty())
        {
          vCDebug(VEIN_API_QML) << "All required entities resolved";
          m_state = ConnectionState::VQ_LOADED;
          emit sigStateChanged(m_state);
        }
      }
      emit sigEntityAvailable(nameFromEntityId(t_entityId));
    }
  }

  int VeinQml::idFromEntityName(const QString &t_entityName) const
  {
    int retVal = -1;
    if(t_entityName.isEmpty() == false)
    {
      foreach(int tmpKey, m_entities.keys())
      {
        EntityComponentMap *eMap = m_entities.value(tmpKey);
        if(eMap->value("EntityName") == t_entityName) ///< @todo remove hardcoded
        {
          retVal = tmpKey;
          break;
        }
      }
    }
    return retVal;
  }

  QString VeinQml::nameFromEntityId(int t_entityId) const
  {
    QString retVal;
    if(m_entities.contains(t_entityId))
    {
      retVal = m_entities.value(t_entityId)->value("EntityName").value<QString>(); ///< @todo remove hardcoded
    }
    return retVal;
  }


  VeinQml *VeinQml::m_staticInstance = 0;
}
