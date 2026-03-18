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
- **Build:** CMake

## Project Structure

```
chat-system/
├── backend/           # TCP server/client networking code
├── frontend/          # Qt/QML application
│   ├── qml/          # QML UI components
│   └── models/       # C++ models for QML
├── chat-server.cpp    # Server source
├── chat-client.cpp    # Client source
└── CMakeLists.txt    # Build configuration
```

## Building

### Prerequisites

**macOS:**
```bash
brew install qt
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt install qt6-base-dev qt6-declarative-dev qt6-networkauth-dev
# Or for Qt 5
sudo apt install qtbase5-dev qtdeclarative5-dev qtmultimedia5-dev
```

### Compile

```bash
mkdir build && cd build
cmake ..
make
```

## Running

### Backend (Phase 1)
```bash
./server    # Start server
./client    # Start client (terminal-based)
```

### Frontend (Phase 2 - Qt GUI)
```bash
./chat-app  # Start GUI application
```

## Roadmap

See [TODO.md](./TODO.md) for development phases and planned features.

## References

- [Qt Documentation](https://doc.qt.io/)
- [Qt Quick Controls 2](https://doc.qt.io/qt-6/qtquickcontrols2-index.html)
- [Blog Tutorial](https://devlogs.xyz/blog/real-time-chat-system)
- [SimpleSocket](https://github.com/markaren/SimpleSocket)

## License

MIT License
