# Build and Run

## Requirements

Linux, a C++17 compiler, and POSIX sockets. Commands use GCC 9 or newer. Port 8080 must be available. On Windows, build and run within a Linux environment such as WSL Ubuntu.

## Build

```bash
g++ -std=c++17 -Wall -Wextra -pedantic server.cpp -pthread -o server
g++ -std=c++17 -Wall -Wextra -pedantic client.cpp -o client
```

## Start the server

```bash
./server
```

## Start clients

Run the following in separate terminals on the same computer:

```bash
./client
```

Each input line receives an echo response. `/quit` receives `Goodbye!` and closes the client connection. End of input also closes the client. Ctrl+C stops the server.

## Connection behavior

- Address: `127.0.0.1:8080`.
- Maximum message length: 4096 bytes, excluding the ending newline.
- Oversized terminal input is rejected by the client.
- An oversized message from another socket client closes that connection.
- Incomplete messages are discarded on disconnection.
- A missing server or occupied port produces a startup error.
- Connections have no idle timeout.
