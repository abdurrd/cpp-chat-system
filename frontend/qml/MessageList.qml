import QtQuick 2.15
import QtQuick.Controls 2.15

/**
 * Message List View
 * Displays all chat messages with auto-scroll
 */
ListView {
    id: messageListView
    
    clip: true
    spacing: 8
    boundsBehavior: Flickable.StopAtBounds
    
    // Scroll to bottom when new messages arrive
    function scrollToBottom() {
        if (count > 0) {
            positionViewAtIndex(count - 1, ListView.End)
        }
    }
    
    // Auto-scroll on model changes
    onCountChanged: scrollToBottom()
    
    delegate: ChatBubble {
        width: messageListView.width
        messageText: model.text
        senderName: model.sender
        isFromMe: model.isFromMe
        timestamp: model.timestamp
        isRead: model.isRead || false
        
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            
            onClicked: {
                // TODO: Show context menu (copy, forward, delete)
                console.log("Right-click on message:", model.text)
            }
        }
    }
    
    // Scrollbar
    ScrollBar.vertical: ScrollBar {
        active: true
        policy: ScrollBar.AsNeeded
    }
}
