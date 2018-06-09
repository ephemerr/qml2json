#include <QtCore>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QThread>
#include <QQuickItem>
#include <QJsonDocument>

static QSet<QString> subbranches = {"success", "fail", "both", "wait_actions", "exec_actions"};

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

enum ObjectType {
    kObjectTypeModel,
    kObjectTypeWaitAction,
    kObjectTypeExecAction,
};

QString qmlFromJsonObj(const QJsonObject *obj, ObjectType type) {
  QString res;
  for(auto i = 0; i < obj->keys().size(); i++) {
    QString key = obj->keys().at(i);
    QJsonValue val = obj->value(key);
    res += (key == "id") ? "model_id: " : key +  ": ";
    if (val.isArray() && subbranches.contains(key)) {
      auto branch = val.toArray();
      for(auto e = branch.begin(); e != branch.end(); e++) {
      }
      res += "[]";
    } else if (val.isObject()) {
    } else {
      QString strval = val.toVariant().toString();
      bool noquotes=true;
      strval.toInt(&noquotes);
      noquotes = noquotes ? noquotes : strval == "false" or strval == "true";
      if (noquotes) {
        res += strval;
      } else {
        res += QString("\"%1\"").arg(strval);
      }
    }
    res += ";";
  }
  return res;
}

QString qmlFromJson(const QJsonObject *obj) {
  QString res = "import QtQuick 2.4;import \"WaitActions\";Model {";
  return res + qmlFromJsonObj(obj, kObjectTypeModel);
}

QString offsetStr(int offset) {
  if (offset <= 0) return "";
  return QString("%1").arg(' ',offset);
}

QString formatQml(QString qml) {
  QString res;
  int offset=0;
  for(int i=0; i<qml.size(); i++) {
    switch(qml[i].toLatin1()) {
        case '{':
          offset += 2;
          res += "{\n" + offsetStr(offset);
          break;
        case ';':
          res += "\n" + offsetStr(offset);
          break;
        case '}':
          offset -= 2;
          res += "}\n" + offsetStr(offset);
          break;
        default:
          res += qml[i];
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
  qDebug() << formatQml(qmlFromJson(&res)).toUtf8().constData();
  return 0;
}



