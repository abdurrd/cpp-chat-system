# Chat System Development Roadmap

> **Goal:** Build a WhatsApp-like cross-platform chat application with GUI

---

## Phase 1: Core Networking (Backend)

- [x] **1.1** Set up basic chat system from blog tutorial (C++ with raw POSIX sockets)
  - Create `server.cpp` - TCP server with `select()` for multiplexing
  - Create `client.cpp` - TCP client with pthread for send/receive
  - Configure CMake build
- [ ] **1.2** Test chat works on same machine (localhost)
- [ ] **1.3** Test chat across different machines on same LAN

## Phase 2: Qt/QML Integration (GUI)

- [] **2.1** Set up Qt development environment
  - Install Qt 6 (or Qt 5.15 LTS)
  - Configure CMake with Qt components
- [ ] **2.2** Create basic QML application window
- [ ] **2.3** Design chat UI components in QML
  - Message list view
  - Chat bubbles (sent/received)
  - Input field with send button
  - Contact/user list sidebar
- [ ] **2.4** Integrate C++ networking with QML
  - Expose C++ classes to QML
  - Signal/slot connections for real-time updates
  - Thread-safe message queue

## Phase 3: WhatsApp-like Features

- [ ] **3.1** User authentication system
  - Login/register UI
  - Session management
- [ ] **3.2** Contact management
  - Add/remove contacts
  - Contact list with online status
- [ ] **3.3** Chat features
  - One-on-one messaging
  - Group chats
  - Message timestamps
  - Read receipts (single/double tick)
- [ ] **3.4** Media support
  - Image sharing
  - File attachments
  - Voice messages

## Phase 4: Production Features

- [ ] **4.1** TLS/SSL encryption (secure communication)
- [ ] **4.2** Message persistence (SQLite database)
- [ ] **4.3** Push notifications
- [ ] **4.4** Cross-platform builds (macOS, Linux, Windows)
- [ ] **4.5** Rate limiting / bot protection

---

## Project Structure

```
chat-system/
├── backend/
│   ├── server.cpp          # TCP server
│   ├── client.cpp          # TCP client (networking logic)
│   └── CMakeLists.txt
├── frontend/
│   ├── main.cpp            # Qt application entry
│   ├── qml/
│   │   ├── Main.qml        # Main application window
│   │   ├── ChatBubble.qml  # Message bubble component
│   │   ├── MessageList.qml # Message list view
│   │   └── InputField.qml  # Message input
│   ├── models/
│   │   └── messagemodel.cpp # QAbstractListModel for messages
│   └── CMakeLists.txt
├── CMakeLists.txt          # Root CMake config
├── TODO.md
└── README.md
```

---

## Reference Materials

- **Blog Tutorial:** https://devlogs.xyz/blog/real-time-chat-system
- **SimpleSocket:** https://github.com/markaren/SimpleSocket (optional abstraction)
- **Qt Documentation:** https://doc.qt.io/
- **Qt Quick Controls 2:** https://doc.qt.io/qt-6/qtquickcontrols2-index.html
- **Source PDF:** `Building a Real-Time Chat System from Scratch...pdf`

---

## Key Concepts to Understand

### TCP/IP Sockets (Backend)
- `socket()`, `bind()`, `listen()`, `accept()`, `connect()`
- `send()`, `recv()`
- File descriptors, `select()` for I/O multiplexing
- Threading with `pthread` or `std::thread`

### Qt/QML (Frontend)
- QML declarative UI
- C++/QML integration (`Q_PROPERTY`, `Q_INVOKABLE`)
- Signal/slot mechanism
- `QAbstractListModel` for message lists
- Qt Quick Controls 2 for modern UI

### Architecture
- MVC pattern (Model-View-Controller)
- Producer-consumer for message queue
- Thread-safe communication between network and UI

---

*Last updated: 2025-03-18*
