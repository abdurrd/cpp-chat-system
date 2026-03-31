# Chat System

A WhatsApp-like cross-platform chat application with group messaging, built in C++ and Qt/QML.

## Features

- **Real-time group messaging** over TCP/IP
- **Multi-group support** - Users can join multiple groups
- **Focus-based sending** - Send to one group at a time
- **Local chat storage** - Messages stored in group-specific files
- **Thread-safe message handling** - No message mixing between groups
- **User authentication** - Login-based system
- **Modern Qt/QML GUI** (Phase 2)

## Tech Stack

- **Backend:** C++17, POSIX Sockets, pthread
- **Frontend:** Qt 6 / Qt 5.15, QML, Qt Quick Controls 2
- **Build:** CMake / Make
- **Protocol:** Custom binary message protocol

---

## Architecture

### Server Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        SERVER                                │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  Main Thread (Authentication & Group Management)    │    │
│  │  - Validates user login                              │    │
│  │  - Creates/joins groups                              │    │
│  │  - Assigns users to group threads                    │    │
│  └───────────────────┬─────────────────────────────────┘    │
│                      │                                       │
│      ┌───────────────┼───────────────┐                      │
│      ▼               ▼               ▼                      │
│  ┌─────────┐   ┌─────────┐   ┌─────────┐                   │
│  │ Group 1 │   │ Group 2 │   │ Group 3 │                   │
│  │ Thread  │   │ Thread  │   │ Thread  │                   │
│  │ fd_set  │   │ fd_set  │   │ fd_set  │                   │
│  │ Clients │   │ Clients │   │ Clients │                   │
│  └─────────┘   └─────────┘   └─────────┘                   │
└─────────────────────────────────────────────────────────────┘
```

### Client Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        CLIENT                                │
│  ┌─────────────────┐   ┌─────────────────────────────────┐  │
│  │  Main Thread    │   │  Receiver Thread                │  │
│  │  - User input   │   │  - Receives all messages        │  │
│  │  - Send msgs    │   │  - Writes to group files        │  │
│  │  - Focus mgmt   │   │  - Thread-safe file access      │  │
│  └─────────────────┘   └─────────────────────────────────┘  │
│                          chats/                             │
│                          ├── general.txt                    │
│                          ├── random.txt                     │
│                          └── project.txt                    │
└─────────────────────────────────────────────────────────────┘
```

### Message Flow

```
Client A                          Server                        Client B
   │                                │                              │
   │──Login: "alice"───────────────▶│                              │
   │◀──Login Success────────────────│                              │
   │                                │                              │
   │──Join Group: "general"────────▶│                              │
   │                                │──Add to group thread────────▶│
   │                                │                              │
   │──Set Focus: "general"─────────▶│                              │
   │                                │                              │
   │──Send: "Hello!"───────────────▶│                              │
   │                                │──Broadcast to group─────────▶│
   │                                │  [Header: general, alice]    │
   │                                │  [Body: "Hello!"]            │
   │                                │                              │
   │                                │                              │──Write to chats/general.txt
   │                                │                              │
```

---

## Message Protocol

### Message Format

Each message has a **fixed-size header** + **variable-length body**:

```
┌──────────────┬──────────────┬──────────────┬──────────────┐
│  Type        │  Group ID    │  Username    │  Msg Length  │
│  (1 byte)    │  (32 bytes)  │  (32 bytes)  │  (4 bytes)   │
└──────────────┴──────────────┴──────────────┴──────────────┘
│                    Message Body (variable)                  │
└─────────────────────────────────────────────────────────────┘
```

### Message Types

```cpp
enum MessageType : uint8_t {
    MSG_LOGIN = 1,          // Client → Server: Login request
    MSG_LOGIN_SUCCESS,      // Server → Client: Login approved
    MSG_LOGIN_FAIL,         // Server → Client: Login failed
    MSG_CREATE_GROUP,       // Client → Server: Create new group
    MSG_JOIN_GROUP,         // Client → Server: Join existing group
    MSG_GROUP_MESSAGE,      // Client → Server: Send message to group
    MSG_LEAVE_GROUP,        // Client → Server: Leave group
    MSG_SET_FOCUS,          // Client → Server: Set active group
};
```

---

## Project Structure

```
chat-system/
├── chat-server/            # Server application
│   ├── main.cpp           # Main server with group thread management
│   ├── Server.hpp         # Server class header
│   └── Group.hpp          # Group structure and thread logic
│
├── chat-client/            # Client application (CLI)
│   ├── main.cpp           # Client entry point
│   ├── Client.cpp         # Client networking logic
│   ├── Client.hpp         # Client class header
│   ├── User.cpp           # User model implementation
│   ├── User.hpp           # User model header
│   └── message.hpp        # Message protocol definitions
│
├── frontend/               # Qt/QML GUI application (Phase 2)
│   ├── main.cpp           # Qt application entry
│   ├── networkclient.h    # Network wrapper for QML
│   ├── resources.qrc      # Qt resource file
│   │
│   ├── models/
│   │   ├── messagemodel.h # Message list model header
│   │   └── messagemodel.cpp
│   │
│   └── qml/
│       ├── Main.qml       # Main chat window
│       ├── ChatBubble.qml # Message bubble component
│       ├── MessageList.qml
│       └── InputField.qml
│
├── CMakeLists.txt         # CMake build configuration
├── Makefile               # Simple Makefile for quick builds
├── README.md
└── TODO.md
```

---

## Building

### Prerequisites

**macOS:**
```bash
brew install qt@6
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt install qt6-base-dev qt6-declarative-dev qt6-tools-dev
```

### Option 1: Using Make (Quick Testing - Phase 1)

```bash
# Build server and client only
make

# Clean
make clean
```

### Option 2: Using CMake (Full Project with Qt)

```bash
mkdir build && cd build
cmake ..
make
```

---

## Running

### Server
```bash
./chat-server/main
# Output: Server listening on port 8888...
```

### Client (CLI)
```bash
./chat-client/main
# Output: Connected to server
#         Enter username: alice
```

### Client (GUI - Phase 2)
```bash
./frontend/chat-app
```

---

## Usage (CLI)

### 1. Login
```
Enter username: alice
Login successful!
```

### 2. Join or Create Group
```
Commands:
  /join <group_name>   - Join existing group
  /create <group_name> - Create new group
  /list                - List all groups
  /focus <group_name>  - Set focus to group (for sending)
  /leave               - Leave current group
  /quit                - Exit client
```

### 3. Send Messages
```
[general] alice: Hello everyone!
[general] bob: Hi alice!
```

Messages are stored in `chats/<group_name>.txt`

---

## Key Design Decisions

### 1. Group-Based Threading
- Each group runs in its **own thread** with dedicated `fd_set`
- Prevents message mixing between groups
- Users can only send to **one group at a time** (focus-based)

### 2. Message Protocol
- **Fixed-size header** ensures clean message boundaries
- **Length field** prevents partial reads
- **Group ID in header** routes messages correctly

### 3. Thread-Safe File Writing
- **One mutex per group file**
- Receiver thread writes to appropriate file based on Group ID
- `std::ios::app` ensures atomic appends

### 4. Focus System
- User can **receive** from all joined groups
- User can only **send** to focused group
- Prevents accidental cross-group messaging

---

## Development Phases

### Phase 1: Core Networking ✅
- [x] Project structure setup
- [x] Message protocol design
- [x] TCP server with `select()` multiplexing
- [x] Group-based thread management
- [x] TCP client with receiver thread
- [ ] Thread-safe file writing
- [ ] User authentication
- [ ] Focus-based group messaging
- [ ] Test localhost communication
- [ ] Test cross-machine (LAN)

### Phase 2: Qt/QML GUI 🚧
- [x] QML UI components (bubbles, input, list)
- [ ] C++/QML integration
- [ ] Real-time message updates from file system
- [ ] Group selection sidebar
- [ ] Focus management in GUI

### Phase 3: Production Features
- [ ] Password-based authentication
- [ ] TLS/SSL encryption
- [ ] Message persistence (SQLite instead of files)
- [ ] Deploy to cloud (Oracle Cloud / VPS)
- [ ] Rate limiting / bot protection

---

## Deployment Options

### Local Testing
```bash
# Terminal 1 - Server
./chat-server/main

# Terminal 2 - Client 1
./chat-client/main

# Terminal 3 - Client 2
./chat-client/main
```

### Cross-Network (Different Networks)

**Option 1: ngrok (Quick)**
```bash
# On server machine
ngrok tcp 8888

# Share tcp:// address with clients
```

**Option 2: Oracle Cloud Free Tier (Production)**
1. Deploy server to Oracle Cloud VM
2. Open port 8888 in security rules
3. Clients connect to public IP

**Option 3: Tailscale (Private Network)**
```bash
# Install on all devices
tailscale up

# Connect using Tailscale IP (100.x.x.x)
```

---

## References

- [Qt Documentation](https://doc.qt.io/)
- [Qt Quick Controls 2](https://doc.qt.io/qt-6/qtquickcontrols2-index.html)
- [Blog Tutorial](https://devlogs.xyz/blog/real-time-chat-system)
- [SimpleSocket](https://github.com/markaren/SimpleSocket)
- [Oracle Cloud Free Tier](https://www.oracle.com/cloud/free/)
- [POSIX Sockets Guide](https://beej.us/guide/bgnet/)

---

## License

MIT License
