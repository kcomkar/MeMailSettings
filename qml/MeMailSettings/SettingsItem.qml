import QtQuick 1.0

import com.nokia.meego 1.0

Row {
    id: settingsItem
    width: parent.width
    height: settingsLabel.height + settingsEditField.height + saveButton.height
            + settingsLabel.anchors.bottomMargin + settingsEditField.anchors.bottomMargin
            + saveButton.anchors.bottomMargin
    anchors.topMargin: 40
    anchors.bottomMargin: 40

    property string key: keyRole
    property string value: valueRole

    signal saveSetting(string settingsKey, string settingsValue)

    Label {
        id: settingsLabel
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 10
        text: keyRole
        style: LabelStyle {
            fontPixelSize: 28
        }
        wrapMode: Text.WordWrap
    }

    TextField {
        id: settingsEditField
        anchors.top: settingsLabel.bottom
        anchors.left: parent.left
        anchors.bottomMargin: 10
        width: parent.width
        text:  valueRole
        focus: false
    }

    Button {
        id: saveButton
        anchors.top: settingsEditField.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 10
        anchors.bottomMargin: 10
        text: "Save setting"
        onClicked: {
            settingsItem.saveSetting(settingsLabel.text, settingsEditField.text)
        }
    }
}