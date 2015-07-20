#ifndef VEINAPIQML_QMLWRAPPER_H
#define VEINAPIQML_QMLWRAPPER_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

namespace VeinApiQml {

  class QmlWrapper
  {
  public:
    QmlWrapper();
    static QObject *getSingletonInstance(QQmlEngine *t_engine, QJSEngine *t_scriptEngine);
    static void registerTypes();
  };

} // namespace VeinApiQml

#endif // VEINAPIQML_QMLWRAPPER_H
