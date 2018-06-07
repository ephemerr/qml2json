#include <QtCore>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QThread>
#include <QQuickItem>



QJsonObject toJson(const QObject *obj) {
  QJsonObject res;
  auto metaObj = obj->metaObject();
  QQuickItem item;
  for (int i = item.metaObject()->propertyCount(); i < metaObj->propertyCount(); ++i) {
        auto propi = metaObj->property(i);
        auto key = QString(propi.name()) != "value" ? propi.name() : "id";
        auto val = propi.read(obj);
        res[key] = QJsonValue::fromVariant(val);
  }
  return res;
}

QJsonArray readArray(QObject *object, const char *name) {
  auto item = object->findChild<QQuickItem*>(name);
  QJsonArray res;
  foreach(auto action,  item->findChildren<QObject*>()) {
    // qDebug() << action->metaObject()->className();
    res.append(toJson(action));
  }
  return res;
}

int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);

  QQmlEngine engine;
  QQmlComponent component(&engine,
          QUrl::fromLocalFile("src/WorkModels/PalmOrCard.qml"));
  QObject *object = component.create();
  auto res = toJson(object);
  res["wait_actions"] = readArray(object,"wait_actions");
  res["exec_actions"] = readArray(object,"exec_actions");
  qDebug() << res;


  return 0;
  // return app.exec();
}
