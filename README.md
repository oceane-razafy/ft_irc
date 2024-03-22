# ft_irc - Internet Relay Chat Server

This 2-people group project is part of the curriculum at 42 school and aims to develop an Internet Relay Chat (IRC) server in C++98. The server is designed to handle multiple clients simultaneously, ensuring non-blocking I/O operations. The communication protocol followed is based on [RFC 2812](https://www.rfc-editor.org/rfc/rfc2812), and the reference client used for evaluation is [irssi 1.2.3](https://github.com/irssi/irssi/tree/1.2.3).  

My main focus was implementing the foundation of the server, and these commands: KICK, INVITE, TOPIC, MODE (only the execution part, not the parsing one), PART, QUIT, PING. Additionally, I implemented test files in the `tests` folder to automatically send requests to our server and visualize its replies.

## Table of Contents

- [Project Description](#project-description)
- [Installation](#installation)
- [Usage](#usage)

## Project Description

The ft_irc project involves developing an IRC server in C++98 that meets specific requirements outlined by the subject `ft_irc.pdf`. The implemented commands provide clients with the necessary functionality to interact with the server, join channels, communicate with other users, and manage channel operations effectively.

For command syntax, please refer to [RFC 2812 protocol](https://www.rfc-editor.org/rfc/rfc2812) and [Irssi documentation](https://irssi.org/documentation/help/).  

### Implemented Commands

This server supports a range of commands to facilitate communication and interaction between clients. The implemented commands include:

#### Basic User Commands

- **PASS**: Allows clients to authenticate with the server using a password.
- **NICK**: Enables clients to set or change their nickname.
- **USER**: Allows clients to specify their username and other user details.
- **JOIN**: Lets clients join a specific channel to participate in group conversations.
- **PRIVMSG**: Allows private messages between users as well as sending messages to channel members.  

#### Channel Operations

- **KICK**: Allows channel operators to eject a client from the channel.
- **INVITE**: Allows channel members to invite a client to join the specific channel.
- **TOPIC**: Enables users to view or modify the topic of a channel.  
- **MODE**: Allows channel operators to change various channel modes, including:
  - `i`: Enable or disable the invite-only mode for the channel.
  - `t`: Set or remove the restrictions of the TOPIC command to channel operators.
  - `k`: Set or remove the channel password.
  - `o`: Grant or revoke channel operator privileges.
  - `l`: Set or remove the user limit for the channel.

#### Additional Commands

- **PART**: Allows clients to leave a channel.
- **QUIT**: Enables clients to disconnect from the server.
- **PING**: Ensures compatibility with irssi 1.2.3 by responding to PING requests with a PONG reply.   

## Installation

To install and run the ft_irc server, follow these steps:

1. Clone the repository to your local machine:

```
git clone <repo url> ft_irc
```

2. Navigate to the project directory:

```
cd ft_irc
```

3. Compile the server using the provided Makefile:

```
make
```

4. The executable file `ircserv` will be created in the `bin` directory.

## Usage

To use the server, follow these instructions:

1. Execute the server with the specified port and password:

```
./ircserv <port> <password>
```

- `<port>`: The listening port for incoming IRC connections.
- `<password>`: The connection password required by clients.

2. Connect IRC clients (our reference is irssi 1.2.3), to the server using the provided port and password.  

```
/connect <server_address> <port> <password>
```

Alternatively, users can manually send commands to the server using `nc` (netcat) tool. However, to properly establish the connection, users must enter the necessary commands (`PASS`, `NICK`, `USER`) to properly be registered on the server.  

```
nc -C localhost <port>
```

3. Interact with the IRC server and clients using standard IRC commands.  
