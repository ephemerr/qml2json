#include <QtCore>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QThread>
#include <QQuickItem>
#include <QJsonDocument>

QJsonObject toJson(const QObject *obj) {
  QJsonObject res;
  auto metaObj = obj->metaObject();
  QQuickItem item;
  for (int i = item.metaObject()->propertyCount(); i < metaObj->propertyCount(); ++i) {
        auto propi = metaObj->property(i);
        QSet<QString> id_names = {"model_id", "value"};
        auto key = id_names.contains(propi.name()) ? "id" : propi.name();
        auto val = propi.read(obj);
        res[key] = QJsonValue::fromVariant(val);
  }
  foreach(auto child,  obj->findChildren<QObject*>(QString(), Qt::FindDirectChildrenOnly)) {
    // qDebug() << child->metaObject()->className();
    QSet<QString> subbranches = {"success", "fail", "both", "wait_actions", "exec_actions"};
    auto branch_name = subbranches.find(child->objectName());
    if (branch_name != subbranches.constEnd()) {
      // qDebug() << *branch_name;
      QJsonArray branch;
      foreach(auto sub_child,  child->findChildren<QObject*>(QString(), Qt::FindDirectChildrenOnly)) {
          branch.append(toJson(sub_child));
      }
      res[*branch_name] = branch;
    }
  }
  return res;
}

int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);

  QQmlEngine engine;
  QQmlComponent component(&engine,
          QUrl::fromLocalFile("src/WorkModels/CardAndPalm.qml"));
  QObject *object = component.create();
  auto res = toJson(object);
  qDebug() << QJsonDocument(res).toJson().constData();

  return 0;
}
