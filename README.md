# Relay

Relay is a prototype multi-user terminal chat application written in C++20. A
single server accepts multiple TCP clients and broadcasts each message to the
other connected clients.

The project currently supports:

- Multiple simultaneous clients using `select()`
- Bidirectional terminal chat
- Length-prefixed messages up to 4096 bytes
- Clean and unexpected client disconnects

Relay is an early-stage learning project. It does not yet provide usernames,
authentication, encryption, message history, or a graphical interface.

## Requirements

- A Linux or other POSIX-like environment
- A C++20 compiler
- CMake 3.28.3 or newer

## Build

```sh
cmake -S . -B build
cmake --build build
```

This produces two executables:

- `build/relay_server`
- `build/relay_client`

## Run

Start the server in one terminal:

```sh
./build/relay_server
```

Start a client in each additional terminal:

```sh
./build/relay_client
```

Clients connect to `127.0.0.1` on TCP port `5150`. Type a message and press
Enter to send it. Enter `/quit` to disconnect.

## Project structure

```text
include/    Public declarations shared by the executables
src/        Client, server, and shared socket implementations
DESIGN.md   Protocol and architecture details
```

## Current limitations

- The server and client addresses are hard-coded.
- The protocol carries plain text without encryption or authentication.
- Messages do not include usernames or other metadata.
- Socket reads and writes are blocking after `select()` reports readiness, so
  a client that sends an incomplete message can stall the server.
- There are no automated tests yet.

See [DESIGN.md](DESIGN.md) for the protocol and implementation design.
