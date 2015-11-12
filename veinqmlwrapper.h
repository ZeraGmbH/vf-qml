#ifndef VEINAPIQML_QMLWRAPPER_H
#define VEINAPIQML_QMLWRAPPER_H

#include "qml-veinentity_global.h"

#include <QObject>

class QQmlEngine;
class QJSEngine;

/**
 * @brief QML Bindings for VeinComponent, VeinEvent and VeinNet
 */
namespace VeinApiQml {

  /**
   * @brief Registers the types VeinApiQml::VeinQml and VeinApiQml::EntityComponentMap with the QML engine
   */
  class QMLVEINENTITYSHARED_EXPORT QmlWrapper
  {
  public:
    QmlWrapper();
    static QObject *getSingletonInstance(QQmlEngine *t_engine, QJSEngine *t_scriptEngine);
    static void registerTypes();
  };

} // namespace VeinApiQml

#endif // VEINAPIQML_QMLWRAPPER_H
