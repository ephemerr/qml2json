import QtQuick 2.4
import "WaitActions"

Model {
  model_id: 0
  name: "Карта + Ладонь"
  WaitActions {
    ActionCard {
      Success {
        ActionPalm {}
        ActionTimeout {}
      }
    }
  }
}
