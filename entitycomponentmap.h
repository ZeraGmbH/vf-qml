#ifndef ENTITYCOMPONENTMAP_H
#define ENTITYCOMPONENTMAP_H

#include "qml-veinentity_global.h"

#include <QQmlPropertyMap>
#include <QJsonObject>

class QEvent;

namespace VeinComponent
{
  class ComponentData;
}


namespace VeinApiQml
{
  /**
   * @brief QML accessible interface for accessing VeinEvent functionality
   */
  class QMLVEINENTITYSHARED_EXPORT EntityComponentMap : public QQmlPropertyMap
  {
    Q_OBJECT

    /// @b away with the stupid default constructor
    EntityComponentMap() = delete;

  public:
    /**
     * @brief EntityComponentMap
     * @param entityIntrospection [in] expects introspection with preinitialized values
     */
    explicit EntityComponentMap(int t_entityId, const QJsonObject &t_entityIntrospection, QObject *t_parent=0);

    enum class DataState : int {
      ECM_NONE = -1, /**< uninitialized */
      ECM_PENDING = 0, /**< introspection is available but values are not initialized */
      ECM_READY = 1, /**< everything is available */
      ECM_REMOVED = 2, /**< the entity has been removed from the remote end */
    };
    Q_ENUMS(DataState)

    Q_PROPERTY(DataState state READ state NOTIFY sigStateChanged)

    /**
     * @brief processComponentData
     * @param t_cData
     */
    void processComponentData(VeinComponent::ComponentData *t_cData);

    DataState state() const;
    /**
     * @brief setState
     * @param t_dataState
     * @note not callable from QML
     */
    void setState(DataState t_dataState);

    Q_INVOKABLE int entityId() const;

  signals:
    void sigSendEvent(QEvent *t_cEvent);
    void sigLoadedChanged(int t_entityId);

    void sigStateChanged(DataState t_state);


  protected:
    /**
     * @brief Intercepts all value changes coming from the qml side and converts them into CommandEvents
     * @return returns the current value (not the changed one) which will not trigger a valueChanged until the change notification arrives
     */
    QVariant updateValue(const QString &t_key, const QVariant &t_newValue) override;

  private:
    /**
     * @brief Parses over the introspection JSON and initializes all values
     */
    void loadEntityData();

    QList<QString> m_pendingValues;
    QJsonObject m_entityIntrospection;
    DataState m_state = DataState::ECM_PENDING;
    int m_entityId=-1;
  };
}
#endif // ENTITYCOMPONENTMAP_H
