#ifndef VEINQML_H
#define VEINQML_H

#include "qml-veinentity_global.h"

#include <ve_eventsystem.h>

#include <QHostAddress>
#include <QHash>
#include <QJsonObject>



class QQmlPropertyMap;

namespace VeinApiQml
{
  class EntityComponentMap;

  /**
   * @brief QML binding to interoperate with entity/component data via VeinApiQml::EntityComponentMap
   */
  class QMLVEINENTITYSHARED_EXPORT VeinQml : public VeinEvent::EventSystem
  {
    Q_OBJECT

  public:
    /**
     * @brief VeinQml
     * @param t_parent
     *
     * @note if used with the QML engine (e.g. qmlRegisterSingletonType<VeinQml>...) then the instance is owned by QML and will also be deleted by the QML engine
     */
    explicit VeinQml(QObject *t_parent = 0);
    ~VeinQml();

    enum class ConnectionState : int {
      VQ_IDLE = 0, /**< the system has been created and is not yet ready */
      VQ_LOADED = 1, /**< the required entities are set and their introspection data has been fetched */
      VQ_DISCONNECTED = 2, /**< the host or the client has closed the connection */
      VQ_ERROR = 3 /**< error state, e.g. a required entity is not available on the server */
    };
    Q_ENUMS(ConnectionState)

    Q_PROPERTY(ConnectionState state READ state NOTIFY sigStateChanged)

    ConnectionState state() const;

    Q_INVOKABLE EntityComponentMap *getEntity(const QString &t_entityName) const;
    Q_INVOKABLE bool hasEntity(const QString &t_entityName) const;

    Q_INVOKABLE EntityComponentMap *getEntityById(int t_id) const;

    /**
     * @brief Required by qmlRegisterSingletonType
     * @return
     * @todo Replace with host specific instances that are created on the fly for each ip address connected to (like getInstanceForHost('192.168.123.123'))
     */
    static VeinQml *getStaticInstance();
    static void setStaticInstance(VeinQml *t_instance);

    Q_INVOKABLE void setRequiredIds(QList<int> t_requiredEntityIds);

    // EventSystem interface
  public:
    /**
     * @todo set up a queue for sent transactional VeinEvent::CommandEvent and compare against notifications / errors
     */
    bool processEvent(QEvent *t_event) override;

  signals:
    void sigStateChanged(ConnectionState t_state);

    /**
     * @brief Notifies on the availability of new entities
     * @note used by the debugger
     * @param t_entityName
     */
    void sigEntityAvailable(QString t_entityName);

  private slots:
    /**
     * @brief checks the required entities and transits in the VQ_LOADED state when all are resolved
     * @param t_entityId
     */
    void onEntityLoaded(int t_entityId);

  private:
    /**
     * @brief Searches the list of entities for the given name
     * @param t_entityName
     * @return
     * @todo a cross reference hash resolving the names to the id should yield more performance with large sets of entities
     * @todo test time memory tradeoff
     */
    int idFromEntityName(const QString &t_entityName) const;
    QString nameFromEntityId(int t_entityId) const;

    /**
     * @brief The state describes if the system is usable (e.g. in the VQ_LOADED state)
     */
    ConnectionState m_state = ConnectionState::VQ_IDLE;

    /**
     * @brief entity id based lookup table for getEntity()
     */
    QHash<int, EntityComponentMap *> m_entities;

    /**
     * @brief Describes the entity ids whose introspection data is required to enter the VQ_LOADED ConnectionState
     */
    QList<int> m_requiredIds;

    /**
     * @brief Describes the resolved ids of the required list
     */
    QList<int> m_resolvedIds;

    /**
     * @brief QML singleton instance
     * @note do not delete from c++
     */
    static VeinQml *s_staticInstance;
  };
}
#endif // VEINQML_H
