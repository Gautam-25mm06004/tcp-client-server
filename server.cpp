#include "network.h"
#include <arpa/inet.h>
#include <exception>
#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>
using namespace std;

mutex outputMutex;

void logMessage(const string& text) {
    lock_guard<mutex> lock(outputMutex);
    cout << text << endl;
}

void handleClient(int clientFd) {
    logMessage("Client connected: " + to_string(clientFd));
    string message;
    while (receiveLine(clientFd, message)) {
        if (message == "/quit") {
            sendLine(clientFd, "Goodbye!");
            break;
        }
        logMessage("Client " + to_string(clientFd) + ": " + message);
        if (!sendLine(clientFd, message)) break;
    }
    close(clientFd);
    logMessage("Client disconnected: " + to_string(clientFd));
}

int main() {
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        cerr << "Could not create socket.\n";
        return 1;
    }
    int reuse = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        cerr << "Could not configure socket.\n";
        close(serverFd);
        return 1;
    }
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(serverFd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        cerr << "Could not bind port 8080. Is another server running?\n";
        close(serverFd);
        return 1;
    }
    if (listen(serverFd, 10) < 0) {
        cerr << "Could not listen.\n";
        close(serverFd);
        return 1;
    }
    logMessage("Listening on 127.0.0.1:8080. Press Ctrl+C to stop.");
    while (true) {
        int clientFd = accept(serverFd, nullptr, nullptr);
        if (clientFd < 0) {
            if (errno == EINTR) continue;
            logMessage("Accept failed.");
            break;
        }
        try {
            thread worker(handleClient, clientFd);
            worker.detach();
        } catch (const exception& error) {
            close(clientFd);
            logMessage(string("Thread error: ") + error.what());
        }
    }
    close(serverFd);
    return 1;
}
