import QtQuick 2.15
import QtQuick.Controls 2.15

/**
 * Chat Bubble Component
 * Reusable message bubble for chat interface
 */
Rectangle {
    id: root
    
    property string messageText: ""
    property string senderName: ""
    property bool isFromMe: false
    property string timestamp: ""
    property bool isRead: false
    
    width: messageRow.width + 20
    height: messageColumn.height + 10
    color: "transparent"
    
    Row {
        id: messageRow
        anchors.left: isFromMe ? parent.right : parent.left
        anchors.right: isFromMe ? parent.right : undefined
        anchors.leftMargin: 10
        spacing: 8
        
        // Avatar (for received messages)
        Rectangle {
            visible: !isFromMe
            width: 30
            height: 30
            radius: 15
            color: "#4ecca3"
            
            Text {
                text: "👤"
                anchors.centerIn: parent
            }
        }
        
        // Message bubble
        Rectangle {
            id: messageBubble
            width: Math.min(messageTextItem.implicitWidth + 16, parent.width - 50)
            height: messageTextItem.implicitHeight + 12
            radius: 12
            color: isFromMe ? "#e94560" : "#0f3460"
            
            Text {
                id: messageTextItem
                text: root.messageText
                color: "#eeeeee"
                wrapMode: Text.WordWrap
                width: parent.width - 16
                anchors.centerIn: parent
                font.pixelSize: 14
            }
        }
    }
    
    // Timestamp and read receipt
    Row {
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: messageRow.bottom
        anchors.topMargin: 2
        spacing: 4
        
        Text {
            text: root.timestamp
            font.pixelSize: 10
            color: "#888888"
        }
        
        Text {
            visible: root.isFromMe
            text: root.isRead ? "✓✓" : "✓"
            font.pixelSize: 10
            color: root.isRead ? "#4ecca3" : "#888888"
        }
    }
}
