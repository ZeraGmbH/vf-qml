#ifndef VEINAPIQML_QMLWRAPPER_H
#define VEINAPIQML_QMLWRAPPER_H

#include "globalIncludes.h"

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
  class VFQML_EXPORT QmlWrapper
  {
  public:
    QmlWrapper();
    static QObject *getSingletonInstance(QQmlEngine *t_engine, QJSEngine *t_scriptEngine);
  };

} // namespace VeinApiQml

#endif // VEINAPIQML_QMLWRAPPER_H
