#ifndef VEINAPIQML_QMLWRAPPER_H
#define VEINAPIQML_QMLWRAPPER_H

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
  class QmlWrapper
  {
  public:
    QmlWrapper();
    static QObject *getSingletonInstance(QQmlEngine *t_engine, QJSEngine *t_scriptEngine);
    static void registerTypes();
  };

} // namespace VeinApiQml

#endif // VEINAPIQML_QMLWRAPPER_H
