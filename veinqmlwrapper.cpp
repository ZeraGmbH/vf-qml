#include "veinqmlwrapper.h"

#include <qqml.h>

#include <veinqml.h>
#include <entitycomponentmap.h>

namespace VeinApiQml {

  QmlWrapper::QmlWrapper()
  {

  }

  QObject *QmlWrapper::getSingletonInstance(QQmlEngine *t_engine, QJSEngine *t_scriptEngine)
  {
    Q_UNUSED(t_engine);
    Q_UNUSED(t_scriptEngine);

    return VeinApiQml::VeinQml::getStaticInstance();
  }

  void QmlWrapper::registerTypes()
  {
    // @uri Vein
    using namespace VeinApiQml;
    qmlRegisterSingletonType<VeinQml>("VeinEntity", 1, 0, "VeinEntity", QmlWrapper::getSingletonInstance);
    qmlRegisterInterface<EntityComponentMap>("EntityComponentMap");//, 1, 0, "VeinEntityMap", QString("VeinEntityMap is not creatable"));
  }
} // namespace VeinApiQml

