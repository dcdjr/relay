# Relay

Relay v0.1 is a prototype multi-user chat system written in C++.

## Architecture

### File structure

- server.cpp
    - Contains all necessary code for the chat server.
- client.cpp
    - Contains all necessary code for the client to connect to the server and send messages.
- common.cpp
    - Any code that may be required for both the server and client.

### Message protocol

For now, the maximum message length will be exactly 4096 bytes.

Every relay message is:
    4 bytes: message length
    N bytes: UTF-8/text payload

## Future Features

- Polished chat with multiple users
- A simple GUI
- Encryption so that the server never sees plaintext messages
