# chatroom

This is a code for a chatroom. There is a server code and a client code. The server must be run (simply by running `./server`) for clients to connect. `MAX_CLIENTS` clients can connect to the server at once. Once connected, the server gives each client a thread to run on. From this, the client run on two total threads, one to read and one to write.

---

Simple goals:
- [x] Clients can send messages to each other.
- [x] Clients run on their own threads in parallel.
- [x] Clients can name themselves on connection. Their defined name will be used to denote to the people in the chat.
- [ ] The format of the messages is readable and makes sense
- [ ] "Smart" string handling
- [ ] Whispering from one client to another
- [ ] Breakout room functionality (?)
