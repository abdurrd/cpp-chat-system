# Server-Side Test Data README

## Directory Structure

```
testing/
├── groups/           # Group chat history files
│   ├── general.txt
│   ├── project.txt
│   └── random.txt
├── users/            # User data and mappings
│   ├── test_users.txt
│   └── user_groups.txt
└── client_dummy/     # Client-side test data
    ├── messages.txt
    └── login_credentials.txt
```

## File Formats

### test_users.txt
```
username:password:user_id
alice:pass123:1001
```

### user_groups.txt
```
user_id:group_file_path
1001:/groups/general.txt
```

### Group Chat Files (general.txt, project.txt, random.txt)
```
[username][HH:MM:SS]: message
[alice][10:30:45]: Hey everyone!
```

### client_dummy/messages.txt
```
group_id:message
1001:Hello from dummy client!
```

## Usage

### Load Test Users (Server-side)
```cpp
std::ifstream users_file("testing/users/test_users.txt");
std::string line;
while (std::getline(users_file, line)) {
    if (line[0] == '#') continue;  // Skip comments
    // Parse: username:password:user_id
}
```

### Load Group Messages (Server-side)
```cpp
std::ifstream group_file("testing/groups/general.txt");
std::string line;
while (std::getline(group_file, line)) {
    // Parse: [username][timestamp]: message
}
```

### Send Dummy Messages (Client-side)
```cpp
std::ifstream msgs("testing/client_dummy/messages.txt");
std::string line;
while (std::getline(msgs, line)) {
    if (line[0] == '#') continue;
    // Parse: group_id:message
    // Send to server periodically
}
```

## Test Scenarios

1. **Login Test**: Use credentials from `test_users.txt`
2. **Group Messaging**: Send messages to groups in `user_groups.txt`
3. **Message History**: Verify messages match format in `groups/*.txt`
4. **Dummy Messages**: Client sends from `client_dummy/messages.txt` every N seconds
