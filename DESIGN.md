# Relay design

Relay v0.1 is a small multi-user chat system built on IPv4 TCP sockets. It has
two command-line executables and a library containing their shared message
framing code.

## Components

### Server

`relay_server` listens on TCP port `5150` on all available network interfaces.
It maintains a collection of connected client sockets and uses `select()` to
wait for either a new connection or data from an existing client.

When the server receives a complete message, it writes the message to standard
output and broadcasts it to every client except the sender. Disconnected or
failed clients are collected during processing and removed after iteration so
that the active client collection is not modified while it is being traversed.

Relevant files:

- `src/server.cpp`: creates, configures, binds, and listens on the server socket
- `src/server_main.cpp`: accepts clients and runs the `select()` event loop

### Client

`relay_client` connects to `127.0.0.1:5150`. Its event loop uses `select()` to
monitor both standard input and the server socket:

- When standard input is ready, the client reads and sends a message.
- When the server socket is ready, the client receives and displays a message.
- Entering `/quit`, reaching end-of-file, or losing the server connection ends
  the client process.

Relevant files:

- `src/client.cpp`: creates the socket and connects it to the server
- `src/client_main.cpp`: handles terminal input and incoming server messages

### Shared networking code

`src/common.cpp` implements the framed-message protocol used by both
executables. `send_all()` and `recv_exact()` handle TCP operations that transfer
fewer bytes than requested. `send_message()` and `recv_message()` build on those
helpers to send and receive complete Relay messages.

## Message protocol

TCP provides a byte stream rather than preserving individual message
boundaries. Relay therefore prefixes every payload with its length:

```text
Offset  Size       Description
0       4 bytes    Unsigned payload length in network byte order
4       N bytes    Text payload
```

Payloads must contain between 1 and 4096 bytes. The current protocol treats the
payload as text, but it does not validate UTF-8 encoding. The terminating null
character used by the receiving program is not transmitted.

## Server event loop

Each server-loop iteration follows these steps:

1. Create a read set containing the listening socket and every client socket.
2. Call `select()` and wait until at least one socket is readable.
3. Accept a new client if the listening socket is ready.
4. Receive and broadcast messages from ready client sockets.
5. Close and remove clients that disconnected or encountered an error.

The design keeps connection management in one thread and avoids waiting on an
inactive socket. The current sockets remain blocking, however. Once a client
starts a framed message, `recv_exact()` waits for the rest of that message. A
slow or malicious client can therefore delay every other client.

## Error and disconnect handling

- Interrupted `select()` calls are retried.
- A zero-byte receive indicates a clean disconnect.
- A disconnect in the middle of a framed message is treated as an error.
- Sends use `MSG_NOSIGNAL`, allowing a failed write to return an error instead
  of terminating the process with `SIGPIPE`.
- Failed clients are de-duplicated before cleanup so their descriptors are not
  closed more than once.

## Planned improvements

Near-term work should focus on correctness and testability:

1. Add automated tests for framing, broadcasting, and disconnect behavior.
2. Use non-blocking sockets with per-client receive state.
3. Make the server address and port configurable.
4. Add structured protocol messages for usernames and join/leave events.

Longer-term possibilities include a graphical client, message persistence,
authentication, and end-to-end encryption.
