import QtQuick 2.4

Item {
  property int value: -1 // id
  property string comment: ""
  property bool gui_ident: true
  property bool ident: false
  property int log_success: 0
  property int log_fail: 0
  property bool log_on_retry: false
  property variant exec_success: []
  property variant exec_fail:  []
}

