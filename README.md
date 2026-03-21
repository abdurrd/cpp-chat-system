# Chat System

A WhatsApp-like cross-platform chat application with GUI built in C++ and Qt/QML.

## Features

- Real-time messaging over TCP/IP
- Modern chat UI with Qt Quick/QML
- Cross-platform (macOS, Linux, Windows)
- Secure communication (TLS support planned)

## Tech Stack

- **Backend:** C++17, POSIX Sockets
- **Frontend:** Qt 6 / Qt 5.15, QML, Qt Quick Controls 2
- **Build:** CMake / Make

## Project Structure

```
chat-system/
├── chat-server/           # Server application
│   └── main.cpp          # TCP server with select() multiplexing
│
├── chat-client/          # Client application (CLI)
│   ├── main.cpp          # Client entry point
│   ├── Client.cpp        # Client networking logic
│   ├── Client.hpp        # Client class header
│   ├── User.cpp          # User model implementation
│   └── User.hpp          # User model header
│
├── frontend/             # Qt/QML GUI application (Phase 2)
│   ├── main.cpp          # Qt application entry
│   ├── networkclient.h   # Network wrapper for QML
│   ├── resources.qrc     # Qt resource file
│   │
│   ├── models/
│   │   ├── messagemodel.h   # Message list model header
│   │   └── messagemodel.cpp # Message model implementation
│   │
│   └── qml/
│       ├── Main.qml         # Main chat window
│       ├── ChatBubble.qml   # Reusable message bubble
│       ├── MessageList.qml  # Message list view
│       └── InputField.qml   # Message input with send button
│
├── CMakeLists.txt        # CMake build configuration
├── Makefile              # Simple Makefile for quick builds
├── README.md
└── TODO.md
```

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

### Option 1: Using Make (Quick Testing)

```bash
# Build server and client only (Phase 1)
make

# Clean
make clean
```

### Option 2: Using CMake (Full Project with Qt)

```bash
mkdir build && cd build
cmake ..
make

# Install (optional)
sudo make install
```

## Running

### Server (Terminal)
```bash
./chat-server/main      # Or use CMake build: ./server
```

### Client - CLI (Terminal)
```bash
./chat-client/main      # Or use CMake build: ./client
```

### Client - GUI (Qt Application)
```bash
./frontend/chat-app     # After building with CMake
```

## Architecture

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Client 1  │    │   Client 2  │    │   Client 3  │
│  (CLI/GUI)  │    │  (CLI/GUI)  │    │  (CLI/GUI)  │
└──────┬──────┘    └──────┬──────┘    └──────┬──────┘
       │                  │                  │
       └──────────────────┼──────────────────┘
                          │
                    ┌─────▼─────┐
                    │  Server   │
                    │  (TCP)    │
                    └───────────┘
```

## Development Phases

### Phase 1: Core Networking ✅
- [x] Project structure setup
- [x] TCP server with `select()` multiplexing
- [x] TCP client with threading
- [x] CMake and Makefile configuration
- [ ] Test localhost communication
- [ ] Test cross-machine (LAN)

### Phase 2: Qt/QML GUI 🚧
- [x] QML UI components (bubbles, input, list)
- [ ] C++/QML integration
- [ ] Real-time message updates
- [ ] Contact list with status

### Phase 3: Production Features
- [ ] User authentication
- [ ] TLS/SSL encryption
- [ ] Message persistence (SQLite)
- [ ] Deploy to cloud (Oracle Cloud / VPS)

## Roadmap

See [TODO.md](./TODO.md) for detailed development plan.

## Deployment Options

### Local Testing
```bash
# Terminal 1 - Server
./chat-server/main

# Terminal 2 - Client
./chat-client/main
```

### Cross-Network (Different Networks)

**Option 1: ngrok (Quick)**
```bash
# On server machine
ngrok tcp 8888

# Share the tcp:// address with clients
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

## References

- [Qt Documentation](https://doc.qt.io/)
- [Qt Quick Controls 2](https://doc.qt.io/qt-6/qtquickcontrols2-index.html)
- [Blog Tutorial](https://devlogs.xyz/blog/real-time-chat-system)
- [SimpleSocket](https://github.com/markaren/SimpleSocket)
- [Oracle Cloud Free Tier](https://www.oracle.com/cloud/free/)

## License

MIT License
