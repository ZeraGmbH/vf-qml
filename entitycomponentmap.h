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
      ECM_READY = 0,
      ECM_REMOVED = 1
    };
    Q_ENUMS(DataState)

    Q_PROPERTY(DataState state READ state NOTIFY sigStateChanged)

    void processComponentData(VeinComponent::ComponentData *t_cData);

    DataState state() const;
    void setState(DataState t_dataState);

    int entityId() const;

  signals:
    void sigSendEvent(QEvent *t_cEvent);
    void sigLoadedChanged(bool t_loaded);

    void sigStateChanged(DataState t_state);

  protected:
    /**
     * @brief Intercepts all value changes coming from the qml side and converts them into CommandEvents
     * @return returns the current value (not the changed one) which will not trigger a valueChanged
     */
    QVariant updateValue(const QString &t_key, const QVariant &t_newValue) override;

  private:
    /**
     * @brief Parses over the introspection JSON and initializes all values
     */
    void loadEntityData();


    QJsonObject m_entityIntrospection;
    DataState m_state = DataState::ECM_READY;
    int m_entityId=-1;
  };
}
#endif // ENTITYCOMPONENTMAP_H
