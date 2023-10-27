# multithread-chat-cli
A command line chat interface, which allows two machines to communicate on the terminal.

This program contains 4 threads:
1. Transmit: Intiializes bytes based on input and sends it over a UDP connection
2. Write: Outputs (writes) the earliest message.
3. Receiver: Recovers bytes and binds socket.
4. Read: Declares bytes through keyboard input

To use the chat, compile all C components using make and run the following command:

```
$ ./s-talk [your port number] [receiver hostname] [receiver port number]
```
