import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

/**
 * Message Input Field
 * Text input with send button and emoji support
 */
RowLayout {
    id: inputField
    
    property alias inputText: messageInput.text
    
    spacing: 10
    height: 60
    
    signal messageSent(string text)
    
    // Emoji picker button
    RoundButton {
        text: "😀"
        Layout.preferredWidth: 40
        Layout.preferredHeight: 40
        highlighted: false
        background: Rectangle {
            color: "#0f3460"
            radius: 20
        }
        
        onClicked: {
            // TODO: Show emoji picker
            console.log("Show emoji picker")
        }
    }
    
    // Text input
    TextField {
        id: messageInput
        Layout.fillWidth: true
        placeholderText: "Type a message..."
        placeholderTextColor: "#888888"
        color: "#eeeeee"
        font.pixelSize: 14
        
        background: Rectangle {
            color: "#0f3460"
            radius: 20
            border.color: "#e94560"
            border.width: messageInput.activeFocus ? 2 : 0
        }
        
        Keys.onPressed: {
            if (event.key === Qt.Key_Return && !event.modifiers) {
                event.accepted = true
                if (messageInput.text.trim().length > 0) {
                    messageSent(messageInput.text)
                    messageInput.text = ""
                }
            }
        }
    }
    
    // Attachment button
    RoundButton {
        text: "📎"
        Layout.preferredWidth: 40
        Layout.preferredHeight: 40
        highlighted: false
        background: Rectangle {
            color: "#0f3460"
            radius: 20
        }
        
        onClicked: {
            // TODO: Show file picker
            console.log("Attach file")
        }
    }
    
    // Send button
    RoundButton {
        text: "➤"
        Layout.preferredWidth: 45
        Layout.preferredHeight: 45
        highlighted: true
        background: Rectangle {
            color: "#e94560"
            radius: 22.5
            
            Behavior on color {
                ColorAnimation { duration: 150 }
            }
        }
        
        // Disable when empty
        enabled: messageInput.text.trim().length > 0
        opacity: enabled ? 1.0 : 0.5
        
        onClicked: {
            if (messageInput.text.trim().length > 0) {
                messageSent(messageInput.text)
                messageInput.text = ""
            }
        }
    }
}
