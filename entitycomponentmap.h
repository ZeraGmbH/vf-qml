#ifndef ENTITYCOMPONENTMAP_H
#define ENTITYCOMPONENTMAP_H

#include "globalIncludes.h"

#include <QQmlPropertyMap>
#include <QJSValue>
#include <QVariantMap>
#include <QUuid>

class QEvent;

namespace VeinComponent
{
  class ComponentData;
  class RemoteProcedureData;
}


namespace VeinApiQml
{
  /**
   * @brief QML accessible interface for accessing VeinEvent functionality
   */
  class VFQML_EXPORT EntityComponentMap : public QQmlPropertyMap
  {
    Q_OBJECT

    /// @b away with the stupid default constructor
    EntityComponentMap() = delete;

  public:
    explicit EntityComponentMap(int t_entityId, const QVariantHash &t_entityIntrospection, QObject *t_parent=nullptr);

    enum class DataState : int {
      ECM_NONE = -1, /**< uninitialized */
      ECM_PENDING = 0, /**< introspection is available but values are not initialized */
      ECM_READY = 1, /**< everything is available */
      ECM_REMOVED = 2, /**< the entity has been removed from the remote end */
    };
    Q_ENUMS(DataState)

    Q_PROPERTY(DataState state READ state NOTIFY sigStateChanged)
    Q_PROPERTY(QStringList remoteProcedures READ getRemoteProcedureList NOTIFY sigRemoteProceduresChanged)

    /**
     * @brief see inline comments
     * @param t_cData
     */
    void processComponentData(VeinComponent::ComponentData *t_cData);

    void processRemoteProcedureData(VeinComponent::RemoteProcedureData *t_rpcData);

    DataState state() const;
    /**
     * @brief calls loadEntityData() if the t_dataState is ECM_PENDING
     * @param t_dataState
     * @note not callable from QML
     */
    void setState(DataState t_dataState);

    Q_INVOKABLE int entityId() const;
    //alias for QQmlPropertyMap::contains
    Q_INVOKABLE bool hasComponent(const QString &t_componentName) const;
    Q_INVOKABLE int propertyCount() const;
    /**
     * @brief Calls remote procedure
     * @param t_procedureName
     * @param t_parameters
     * @return id of the result to expect
     */
    Q_INVOKABLE QUuid invokeRPC(const QString &t_procedureName, const QVariantMap &t_parameters);
    Q_INVOKABLE void cancelRPCInvokation(QUuid t_identifier);

    Q_INVOKABLE QList<QString> getRemoteProcedureList() const;

  signals:
    void sigSendEvent(QEvent *t_cEvent);
    void sigEntityComplete(int t_entityId);

    void sigStateChanged(DataState t_state);

    /**
     * @brief The signal is emitted when a pending RPC call finishes
     * @param t_identifier unique identifier of the rpc call
     * @param t_resultData the result including the resultCode and eventual data
     */
    void sigRPCFinished(QUuid t_identifier, const QVariantMap &t_resultData);
    /**
     * @brief The signal is emitted when a pending RPC call transmits progress information or streams data
     * @param t_identifier unique identifier of the rpc call
     * @param t_progressData the progress information or streamed data
     */
    void sigRPCProgress(QUuid t_identifier, const QVariantMap &t_progressData);
    void sigRemoteProceduresChanged(QStringList t_procedureList);

  protected:
    /**
     * @brief Intercepts all value changes coming from the qml side and converts them into CommandEvents
     * @note updateValue is NOT called when changes are made by calling insert() or clear() - it is only emitted when a value is updated from QML.
     * @return returns the current value (not the changed one) which will not trigger a valueChanged until the change notification arrives
     */
    QVariant updateValue(const QString &t_key, const QVariant &t_newValue) override;

  private:
    /**
     * @brief Parses over the introspection JSON and initializes all values
     */
    void loadEntityData();

    /**
     * @brief Required component data to enter the ECM_READY state
     */
    QList<QString> m_pendingValues;

    /**
     * @brief list of available remote procedures
     */
    QList<QString> m_registeredRemoteProcedures;

    /**
     * @brief Tracks rpc calls made by this instance QString is the procedure name
     */
    QHash<QUuid, QString> m_pendingRPCCallbacks;

    /**
     * @brief QVariantMap representation of the entity layout
     */
    const QVariantHash m_entityIntrospection;

    /**
     * @brief intern state
     */
    DataState m_state = DataState::ECM_PENDING;

    /**
     * @brief entity id
     */
    const int m_entityId;
  };
}
#endif // ENTITYCOMPONENTMAP_H
