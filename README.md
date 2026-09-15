# Multithreaded TCP Client-Server

## What it does

This C++17 application provides a TCP echo server and client for Linux. Multiple clients can connect to the server, send messages, and receive their own messages back. Clients are handled independently, so one waiting or disconnected client does not block the others.

The application runs on `127.0.0.1:8080` and supports messages up to 4096 bytes per line. It includes connection error handling and clean client disconnection. Broadcasting, authentication, encryption, and remote host configuration are outside its scope.

## How it works

The server creates a listening socket, binds it to the local address, and waits for connections. Each accepted connection gets a separate socket and a worker thread. The main thread continues accepting clients while workers handle message exchanges.

Each worker receives a complete message, sends it back, and waits for the next message. A newline marks the end of a message. Shared helper functions handle partial sends and messages that arrive in separate pieces or together.

A mutex protects server log output so different threads do not mix their printed messages. Each worker owns its client's socket and closes it after a quit request, end of input, or connection failure.

Client capacity depends on available thread resources. Stopping the server ends the process without waiting for active clients to finish.
