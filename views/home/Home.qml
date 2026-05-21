import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "./components"
// import DB_LocalStorage

Item {
    anchors.fill: parent
    width: 600
    height: 400

    // for testing with QML preview
    Layout.minimumHeight: 400
    Layout.minimumWidth: 400
    Layout.preferredHeight: 400
    Layout.preferredWidth: 600
    //



    // Home displays user's vaults
    // Allow add/removal of new, customizable vaults

    // FIX: vault content overflows into pane if the window size is too small (set window restriction or modify content layout behavior)

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        handle: Rectangle {
            implicitWidth: 3
            color: "#888"
        }

        HomePane {
            SplitView.minimumWidth: 100
            SplitView.preferredWidth: 200
            SplitView.maximumWidth: 300
        }

        Rectangle {
            id: vaultContent
            SplitView.fillWidth: true

            Text {
                anchors.centerIn: parent
                text: "content"
            }
        }
    }

    Rectangle {
        id: homeUtilityFooter
        width: parent.width
        anchors.bottom: parent.bottom
        height: 25

    }
}
