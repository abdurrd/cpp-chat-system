import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    visible: true
    width: 900
    height: 650
    title: "Chat System"
    color: "#1a1a2e"

    // TODO: Connect to C++ backend
    // property var networkClient
    // property var messageModel

    header: ToolBar {
        background: Rectangle {
            color: "#16213e"
        }
        
        Label {
            text: "💬 Chat System"
            font.pixelSize: 20
            font.bold: true
            color: "#e94560"
            anchors.centerIn: parent
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Left Sidebar - Contact List
        Rectangle {
            Layout.preferredWidth: 250
            Layout.fillHeight: true
            color: "#16213e"

            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Label {
                    text: "Contacts"
                    font.pixelSize: 16
                    font.bold: true
                    color: "#e94560"
                }

                // TODO: Contact list
                Rectangle {
                    width: parent.width
                    height: 40
                    color: "#0f3460"
                    radius: 5

                    Row {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        spacing: 10

                        Rectangle {
                            width: 30
                            height: 30
                            radius: 15
                            color: "#4ecca3"

                            Text {
                                text: "🟢"
                                anchors.centerIn: parent
                            }
                        }

                        Text {
                            text: "User 1"
                            font.pixelSize: 14
                            color: "#eeeeee"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }

        // Right Side - Chat Area
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1a1a2e"

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // Message List
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#1a1a2e"

                    // TODO: Replace with ListView bound to messageModel
                    ListView {
                        id: messageList
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 8
                        clip: true

                        // Placeholder messages
                        model: ListModel {
                            ListElement {
                                text: "Hey! How's it going?"
                                sender: "User 1"
                                isFromMe: "false"
                                timestamp: "10:30 AM"
                            }
                            ListElement {
                                text: "Pretty good! Working on a chat app"
                                sender: "Me"
                                isFromMe: "true"
                                timestamp: "10:31 AM"
                            }
                        }

                        delegate: Rectangle {
                            width: messageList.width
                            height: messageBubble.height + 10
                            color: "transparent"

                            Row {
                                id: messageBubble
                                anchors.left: model.isFromMe === "true" ? parent.right : parent.left
                                anchors.right: model.isFromMe === "true" ? parent.right : undefined
                                anchors.leftMargin: 10
                                spacing: 5

                                Rectangle {
                                    width: messageText.width + 20
                                    height: messageText.height + 10
                                    radius: 10
                                    color: model.isFromMe === "true" ? "#e94560" : "#0f3460"

                                    Text {
                                        id: messageText
                                        text: model.text
                                        color: "#eeeeee"
                                        wrapMode: Text.WordWrap
                                        width: parent.width - 20
                                        anchors.centerIn: parent
                                    }
                                }
                            }

                            Text {
                                text: model.timestamp
                                font.pixelSize: 10
                                color: "#888888"
                                anchors.right: parent.right
                                anchors.rightMargin: 10
                                anchors.top: messageBubble.bottom
                            }
                        }
                    }
                }

                // Input Field
                Rectangle {
                    Layout.preferredHeight: 60
                    Layout.fillWidth: true
                    color: "#16213e"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        TextField {
                            id: messageInput
                            Layout.fillWidth: true
                            placeholderText: "Type a message..."
                            placeholderTextColor: "#888888"
                            color: "#eeeeee"
                            background: Rectangle {
                                color: "#0f3460"
                                radius: 20
                            }

                            Keys.onPressed: {
                                if (event.key === Qt.Key_Return && !event.modifiers) {
                                    event.accepted = true
                                    // TODO: Send message
                                    console.log("Send:", messageInput.text)
                                    messageInput.text = ""
                                }
                            }
                        }

                        RoundButton {
                            text: "➤"
                            Layout.preferredWidth: 45
                            Layout.preferredHeight: 45
                            highlighted: true
                            background: Rectangle {
                                color: "#e94560"
                                radius: 22.5
                            }

                            onClicked: {
                                // TODO: Send message
                                console.log("Send:", messageInput.text)
                                messageInput.text = ""
                            }
                        }
                    }
                }
            }
        }
    }
}
