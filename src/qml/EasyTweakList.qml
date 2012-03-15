import QtQuick 1.1
import com.nokia.meego 1.0

ListView {
    id: easyTweakList
    property int selectedId: 0
    anchors.fill: parent

    Component {
        id: headerComponent
        Item{
            width: parent.width
            height: childrenRect.height + foldersListLabel.anchors.topMargin + separator.anchors.topMargin
            Label {
                id: foldersListLabel
                anchors.top: parent.top
                anchors.topMargin: 10
                text: qsTr("Easy Tweaks")
                color: "white"
                font.pixelSize: 32
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
            Image {
                id: separator
                anchors.top: foldersListLabel.bottom
                anchors.topMargin: 8
                anchors {
                    right: parent.right
                    left: parent.left
                }
                source: "image://theme/meegotouch-groupheader-inverted-background"
            }
        }
    }

    header: Item{
        id: headerItem
        width: parent.width
        height: easyTweakHeaderLabel.height + separator.height + easyTweakHeaderLabel.anchors.topMargin + separator.anchors.topMargin
        Label {
            id: easyTweakHeaderLabel
            anchors.top: parent.top
            anchors.topMargin: 10
            text: qsTr("Easy Tweak Mode")
            color: "white"
            font.pixelSize: 32
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
        Image {
            id: separator
            anchors.top: easyTweakHeaderLabel.bottom
            anchors.topMargin: 8
            anchors {right: parent.right ; left: parent.left}
            source: "image://theme/meegotouch-groupheader-inverted-background"
        }
    }

    model: easyTweakModel

    delegate: EasyTweakItem {
        onItemClicked: {
            easyTweakModel.saveBoolSetting(index, checked);
        }
    }

    onSelectedIdChanged: {
        easyTweakModel.setAccountId(selectedId);
    }
}