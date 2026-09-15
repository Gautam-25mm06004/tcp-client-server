#include "network.h"
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
using namespace std;

int main() {
    int clientFd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFd < 0) {
        cerr << "Could not create socket.\n";
        return 1;
    }
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(clientFd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        cerr << "Connection failed. Start the server first.\n";
        close(clientFd);
        return 1;
    }
    cout << "Connected. Type a message or /quit. Maximum 4096 bytes per line.\n";
    string message, reply;
    int status = 0;
    while (cout << "> " && getline(cin, message)) {
        if (message.size() > 4096) {
            cout << "Message too long. Use at most 4096 bytes.\n";
            continue;
        }
        if (!sendLine(clientFd, message) || !receiveLine(clientFd, reply)) {
            cerr << "Connection closed or network error.\n";
            status = 1;
            break;
        }
        cout << "Server: " << reply << '\n';
        if (message == "/quit") break;
    }
    shutdown(clientFd, SHUT_WR);
    close(clientFd);
    return status;
}
