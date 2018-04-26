import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1
import analytics 0.1

ApplicationWindow {
    id: root
    title: "QtQuick App"

    width: mainLayout.implicitWidth + 2 * margin
    height: mainLayout.implicitWidth + 2 * margin
    minimumWidth: mainLayout.Layout.minimumWidth + 2 * margin
    minimumHeight: mainLayout.Layout.minimumHeight + 2 * margin
    visible: true

    // TODO: Please change this id to yours
    property string defaultTrackingId: "UA-53395376-1"
    property int margin: 11

    Component.onDestruction: {
        tracker.endSession()
    }

    Tracker {
        id: tracker
        logLevel: Tracker.Debug
        sendInterval: 20*1000
        viewportSize: qsTr("%1x%2").arg(root.width).arg(root.height)
        trackingID: defaultTrackingId
    }

    menuBar: MenuBar {
        Menu {
            title: "&File"
            MenuItem {
                text: "&Quit"
                shortcut: "CTRL+Q"
                onTriggered: Qt.quit()
            }
        }
    }

    statusBar: StatusBar {
        RowLayout {
            Label {
                text: "Sending data ..."
                visible: tracker.isSending
            }
        }
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: margin

        GroupBox {
            Layout.fillWidth: true
            title: "General information"

            GridLayout {
                id: rowLayout
                anchors.fill: parent
                anchors.margins: margin
                columns: 2

                Label { text: "Tracking ID" }
                TextField {
                    Layout.fillWidth: true
                    text: tracker.trackingID
                    onTextChanged: tracker.trackingID = text
                }

                Label { text: "Viewport Size" }
                TextField {
                    Layout.fillWidth: true
                    readOnly: true
                    text: tracker.viewportSize
                }


                Label { text: "Language" }
                TextField {
                    Layout.fillWidth: true
                    readOnly: true
                    text: tracker.language
                }


                Label { text: "Send Interval" }
                TextField {
                    Layout.fillWidth: true
                    readOnly: true
                    text: tracker.sendInterval
                }
            }
        }

        GroupBox {
            Layout.fillWidth: true
            title: "Fun box"

            ColumnLayout {
                anchors.fill: parent

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: "Trigger event"
                        onClicked: tracker.sendEvent("ui_event", "button_press", text)
                    }

                    Button {
                        text: "Trigger exception"
                        onClicked: tracker.sendException("some exception", "whatever")
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: "Change current screen name to:"
                        onClicked: tracker.sendScreenView(screenName.text)
                    }

                    TextField {
                        id: screenName
                        Layout.fillWidth: true
                        text: "MainWindow"
                    }
                }
            }
        }
    }
}
