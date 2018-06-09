#include <QtCore>
#include <QCoreApplication>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QThread>
#include <QQuickItem>
#include <QJsonDocument>

static QMap<QString,QString> subbranches = {
  { "success"      , "Success"     },
  { "fail"         , "Fail"        },
  { "both"         , "Both"        },
  { "wait_actions" , "WaitActions" },
  { "exec_actions" , "ExecActions" },
};

enum ActionId {
  kActionExeckBlock   = -2,
  kActionNone         = -1,
  kActionKeyboard     = 0,
  kActionCard         = 1,
  kActionPin          = 2,
  kActionGroupCode    = 3,
  kActionPalm         = 4,
  kActionOut          = 5,
  kActionWiegand      = 6,
  kActionVROut        = 7,
  kActionIn           = 8,
  kActionTimeout      = 9,
  kActionCheckIOState = 10,
  kActionEnroll       = 11,
  kActionDepositBox   = 12,
  kActionTimeInterval = 13,
  kActionCheckJS      = 23,
  kActionMessage      = 24,   // feature/message
  kActionCardTemplate = 25,
};

static QMap<int,QString> action_name = {
  { kActionExeckBlock   , "ActionExeckBlock"  },
  { kActionNone         , "ActionNone"        },
  { kActionKeyboard     , "ActionKeyboard"    },
  { kActionCard         , "ActionCard"        },
  { kActionPin          , "ActionPin"         },
  { kActionGroupCode    , "ActionGroupCode"   },
  { kActionPalm         , "ActionPalm"        },
  { kActionOut          , "ActionOut"         },
  { kActionWiegand      , "ActionWiegand"     },
  { kActionVROut        , "ActionVROut"       },
  { kActionIn           , "ActionIn"          },
  { kActionTimeout      , "ActionTimeout"     },
  { kActionCheckIOState , "ActionCheckIOState"},
  { kActionEnroll       , "ActionEnroll"      },
  { kActionDepositBox   , "ActionDepositBox"  },
  { kActionTimeInterval , "ActionTimeInterval"},
  { kActionCheckJS      , "ActionCheckJS"     },
  { kActionMessage      , "ActionMessage"     },
  { kActionCardTemplate , "ActionCardTemplate"},
};

QJsonObject toJson(const QObject *obj) {
  // qDebug() << obj->metaObject()->className();
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
    int branch_index = subbranches.keys().indexOf(child->objectName());
    if (branch_index != -1) {
      auto branch_name = subbranches.keys().at(branch_index);
      QJsonArray branch;
      foreach(auto sub_child,  child->findChildren<QObject*>(QString(), Qt::FindDirectChildrenOnly)) {
          branch.append(toJson(sub_child));
      }
      res[branch_name] = branch;
    }
  }
  return res;
}

enum ObjectType {
    kObjectTypeModel,
    kObjectTypeWaitAction,
    kObjectTypeExecAction,
};


QString jsonValToStr(const QJsonValue &val) {
  QString res;
  QString strval = val.toVariant().toString();
  bool noquotes=true;
  strval.toInt(&noquotes);
  noquotes = noquotes ? noquotes : strval == "false" or strval == "true";
  if (noquotes) {
    res += strval;
  } else {
    res += QString("\"%1\"").arg(strval);
  }
  return res;
}

QString qmlFromJsonObj(const QJsonObject *obj, ObjectType) {
  QString res;
  for(auto i = 0; i < obj->keys().size(); i++) {
    QString key = obj->keys().at(i);
    QJsonValue val = obj->value(key);
    if (!subbranches.contains(key)) {
      res += (key == "id") ? "model_id: " : key +  ": ";
      if (val.isArray()) {
        auto branch = val.toArray();
        for(auto e = branch.begin(); e != branch.end(); e++) {
          res += jsonValToStr(*e);
        }
      } else {
        res += jsonValToStr(val);
      }
    } else {
      res += QString("%1 {").arg(subbranches[key]);
      auto branch = val.toArray();
      for(auto e = branch.begin(); e != branch.end(); e++) {
        auto branch_type = (key  == "exec_actions") ? kObjectTypeExecAction : kObjectTypeWaitAction;
        QJsonObject branch_obj = e->toObject();
        QString node = action_name.value(branch_obj["id"].toInt());
        res += QString("%1 {%2}").arg(node).arg(qmlFromJsonObj(&branch_obj,branch_type));
      }
      res += "}";
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
        // case '[':
        //   offset += 4;
        //   res += "[\n" + offsetStr(offset);
        //   break;
        case '{':
          offset += 4;
          res += "{\n" + offsetStr(offset);
          break;
        case ';':
          res += "\n" + offsetStr(offset);
          break;
        case '}':
          offset -= 4;
          res += "}\n" + offsetStr(offset);
          break;
        // case ']':
        //   offset -= 4;
        //   res += "]\n" + offsetStr(offset);
        //   break;
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



