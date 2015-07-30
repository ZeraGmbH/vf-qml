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

  class QMLVEINENTITYSHARED_EXPORT VeinQml : public VeinEvent::EventSystem
  {
    Q_OBJECT

  public:
    explicit VeinQml(QObject *t_parent = 0);

    enum class ConnectionState : int {
      VQ_IDLE = 0,
      VQ_LOADED = 1,
      VQ_DISCONNECTED = 2,
      VQ_ERROR = 3
    };
    Q_ENUMS(ConnectionState)

    Q_PROPERTY(ConnectionState state READ state NOTIFY sigStateChanged)

    ConnectionState state() const;

    Q_INVOKABLE EntityComponentMap *getEntity(const QString &t_entityName) const;

    /**
     * @brief Required by qmlRegisterSingletonType
     * @return
     */
    static VeinQml *getStaticInstance();
    static void setStaticInstance(VeinQml *t_instance);

    Q_INVOKABLE void setRequiredIds(QList<int> t_requiredEntityIds);

    // EventSystem interface
  public:
    /**
     * @todo set up a queue for sent transaction events and compare against notifications / errors
     */
    bool processEvent(QEvent *t_event) override;

  signals:
    void sigStateChanged(ConnectionState t_state);

  public slots:
    void connectToServer(QHostAddress t_hostAddress, quint16 t_port);
    /**
     * @brief retries the connection to the server
     * @note currently unused?
     */
    void reconnect();

  private:
    int idFromEntityName(const QString &t_entityName) const;

    ConnectionState m_state = ConnectionState::VQ_IDLE;

    QHostAddress m_lastConnection;
    int m_lastPort = -1;

    QHash<int, EntityComponentMap *> m_entities;
    QList<int> m_requiredIds;

    static VeinQml *m_staticInstance;
  };
}
#endif // VEINQML_H
