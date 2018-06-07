import QtQuick 2.4

Item {
  property int model_id: -1
  property string name: "unnamed"
  property string style: "StandardStatusWindow.qml"
  Loader { id: wait_actions }
  Loader { id: exec_actions }
}
