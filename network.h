#pragma once
#include <cerrno>
#include <string>
#include <sys/socket.h>
using namespace std;

inline bool sendLine(int socketFd, const string& message) {
    string data = message + '\n';
    size_t sent = 0;
    while (sent < data.size()) {
        ssize_t count = send(socketFd, data.data() + sent, data.size() - sent, MSG_NOSIGNAL);
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0) return false;
        sent += static_cast<size_t>(count);
    }
    return true;
}

inline bool receiveLine(int socketFd, string& message) {
    message.clear();
    while (true) {
        char ch;
        ssize_t count = recv(socketFd, &ch, 1, 0);
        if (count < 0 && errno == EINTR) continue;
        if (count <= 0) return false;
        if (ch == '\n') return true;
        if (message.size() == 4096) return false;
        message += ch;
    }
}
