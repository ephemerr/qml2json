import QtQuick 2.4
import "WaitActions"

Model {
  model_id: 0
  name: "Ладонь или Карта"
  WaitActions {
    ActionCard {}
    ActionPalm {}
  }
}
