# Chat System Development Roadmap

> **Goal:** Build a WhatsApp-like cross-platform chat application with group messaging and GUI

---

## Phase 1: Core Networking (Backend) ✅

### 1.1 Message Protocol Design
- [x] Define `MessageHeader` structure (fixed-size: 69 bytes)
- [x] Define `MessageType` enum
- [x] Implement message serialization/deserialization
- [x] Handle message boundaries (header + body)

### 1.2 Server Implementation
- [x] Main thread for authentication
  - [x] Login validation
  - [x] Session management
- [x] Group thread management
  - [x] One thread per group
  - [x] Dedicated `fd_set` per group
  - [x] Client assignment to groups
- [x] Message broadcasting
  - [x] Broadcast to all group members
  - [x] Skip sender optimization
- [x] Focus system
  - [x] Track user's focused group
  - [x] Only allow sending to focused group

### 1.3 Client Implementation
- [x] Receiver thread
  - [x] Read message header first
  - [x] Read exact body length
  - [x] Thread-safe file writing
- [x] Local chat storage
  - [x] One file per group (`chats/<group>.txt`)
  - [x] Mutex per group file
  - [x] Format: `[username][timestamp]: message`
- [x] Focus management
  - [x] `/focus <group>` command
  - [x] Send only to focused group
- [x] User commands
  - [x] `/join`, `/create`, `/list`, `/leave`, `/quit`

### 1.4 Testing
- [ ] Localhost testing (multiple clients)
- [ ] Cross-machine testing (LAN)
- [ ] Message ordering verification
- [ ] No message mixing between groups

---

## Phase 2: Qt/QML Integration (GUI) 🚧

### 2.1 Qt Development Environment
- [ ] Install Qt 6 (or Qt 5.15 LTS)
- [ ] Configure CMake with Qt components
- [ ] Set up Qt Creator or VSCode Qt tools

### 2.2 QML UI Components
- [x] Main.qml - Main chat window
- [x] ChatBubble.qml - Reusable message bubble
- [x] MessageList.qml - Message list view
- [x] InputField.qml - Message input with send button
- [ ] GroupSidebar.qml - Group list with online status
- [ ] LoginDialog.qml - User authentication

### 2.3 C++/QML Integration
- [ ] MessageModel class (QAbstractListModel)
  - [ ] Load messages from group files
  - [ ] Real-time updates via signals/slots
  - [ ] Group switching
- [ ] NetworkClient wrapper
  - [ ] Expose to QML
  - [ ] Signal/slot for message received
  - [ ] Thread-safe communication
- [ ] FocusManager
  - [ ] Track current group in QML
  - [ ] Update server on focus change

### 2.4 GUI Features
- [ ] Real-time message display
- [ ] Group selection sidebar
- [ ] Unread message indicators
- [ ] Timestamp formatting
- [ ] Scroll to bottom on new message
- [ ] Emoji support in input

---

## Phase 3: WhatsApp-like Features

### 3.1 User Authentication
- [ ] Password-based registration
- [ ] Login system with session tokens
- [ ] Remember me functionality
- [ ] Logout and session cleanup

### 3.2 Contact Management
- [ ] Add contacts by username
- [ ] Contact list with online/offline status
- [ ] Block/unblock users
- [ ] Contact search

### 3.3 Advanced Chat Features
- [ ] One-on-one messaging (private chats)
- [ ] Group admin controls
  - [ ] Kick users
  - [ ] Mute users
  - [ ] Invite links
- [ ] Message timestamps (visible in bubbles)
- [ ] Read receipts (✓ / ✓✓)
- [ ] Typing indicators
- [ ] Message editing/deletion

### 3.4 Media Support
- [ ] Image sharing
  - [ ] Thumbnail generation
  - [ ] Image viewer
- [ ] File attachments
  - [ ] File picker
  - [ ] Download manager
- [ ] Voice messages
  - [ ] Record audio
  - [ ] Playback controls

---

## Phase 4: Production Features

### 4.1 Security
- [ ] TLS/SSL encryption (OpenSSL)
  - [ ] Secure server-client communication
  - [ ] Certificate validation
- [ ] Message encryption (end-to-end optional)
- [ ] Secure password storage (bcrypt)

### 4.2 Data Persistence
- [ ] SQLite database
  - [ ] User credentials
  - [ ] Message history
  - [ ] Group memberships
- [ ] Database migrations
- [ ] Backup/restore functionality

### 4.3 Performance & Scalability
- [ ] Connection pooling
- [ ] Message queuing (Redis)
- [ ] Load balancing (multiple server instances)
- [ ] Database indexing

### 4.4 Deployment
- [ ] Docker containerization
- [ ] Deploy to Oracle Cloud / VPS
- [ ] CI/CD pipeline
- [ ] Monitoring and logging

### 4.5 Protection & Moderation
- [ ] Rate limiting per user
- [ ] Bot detection (CAPTCHA)
- [ ] Spam filtering
- [ ] Report system
- [ ] Auto-moderation rules

### 4.6 Cross-Platform Builds
- [ ] macOS app bundle
- [ ] Linux AppImage / Flatpak
- [ ] Windows installer
- [ ] Auto-updater

---

## Architecture Diagrams

### Server Thread Model
```
Main Thread (Authentication)
    │
    ├── Group Thread 1 (fd_set, clients[])
    │       ├── Client A (sender)
    │       ├── Client B (receiver)
    │       └── Client C (receiver)
    │
    ├── Group Thread 2 (fd_set, clients[])
    │       ├── Client A (receiver)
    │       └── Client D (sender)
    │
    └── Group Thread 3 (fd_set, clients[])
            └── Client E (sender/receiver)
```

### Client File Writing
```
Receiver Thread
    │
    ├── Mutex: general.txt
    │       └── Write: [alice][10:30:45]: Hello!
    │
    ├── Mutex: random.txt
    │       └── Write: [bob][10:31:02]: Hi there!
    │
    └── Mutex: project.txt
            └── Write: [charlie][10:31:15]: Update on...
```

---

## Reference Materials

- **Blog Tutorial:** https://devlogs.xyz/blog/real-time-chat-system
- **SimpleSocket:** https://github.com/markaren/SimpleSocket
- **Qt Documentation:** https://doc.qt.io/
- **Qt Quick Controls 2:** https://doc.qt.io/qt-6/qtquickcontrols2-index.html
- **POSIX Sockets:** https://beej.us/guide/bgnet/
- **Oracle Cloud Free Tier:** https://www.oracle.com/cloud/free/
- **Source PDF:** `Building a Real-Time Chat System from Scratch...pdf`

---

## Key Concepts to Master

### Backend (Networking)
- TCP/IP sockets (`socket()`, `bind()`, `listen()`, `accept()`)
- I/O multiplexing with `select()`
- Multi-threading with `pthread`
- Message protocol design
- Thread synchronization (mutexes)
- File I/O with locking

### Frontend (Qt/QML)
- QML declarative UI
- C++/QML integration (`Q_PROPERTY`, `Q_INVOKABLE`)
- Signal/slot mechanism
- `QAbstractListModel` for message lists
- Qt Quick Controls 2
- File system watchers

### Architecture
- Client-server model
- Group-based thread isolation
- Focus-based messaging
- Producer-consumer pattern
- Thread-safe file operations
- MVC pattern for GUI

---

## Current Status

**Phase:** 1 (Backend) - 90% Complete

**Next Steps:**
1. Complete localhost testing
2. Test cross-machine communication
3. Start Phase 2 (Qt/QML GUI)

---

*Last updated: 2025-03-21*
