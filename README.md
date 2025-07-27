# TCP Connection Simulation

This project demonstrates a basic TCP client-server interaction using C++ sockets, allowing you to observe the TCP 3-way handshake, data transfer, and 4-way handshake using `tcpdump`.

## Project Files

  * `server.cpp`: The C++ source code for the TCP server.
  * `client.cpp`: The C++ source code for the TCP client.

## How to Compile

You can compile both the server and client applications using a C++ compiler like g++.

1.  **Open your terminal.**
2.  **Navigate to the directory** where you saved `server.cpp` and `client.cpp`.
3.  **Compile the server:**
    ```bash
    g++ server.cpp -o server
    ```
4.  **Compile the client:**
    ```bash
    g++ client.cpp -o client
    ```

This will create two executable files: `server` and `client`.

## How to Run the Simulation

To observe the TCP connection, you'll need three separate terminal windows.

### Terminal 1: Run `tcpdump`

First, start `tcpdump` to capture network traffic on the loopback interface (`lo`) for port `8080`. The `-vvv` flag provides very verbose output, and `-X` prints the packet data in hex and ASCII.

```bash
sudo tcpdump -i lo tcp port 8080 -vvv -X
```

  * You will be prompted for your `sudo` password.
  * `tcpdump` will start listening and outputting information about packets that match the filter. It will wait for traffic.

### Terminal 2: Start the Server

In a **second terminal window**, start your server application.

```bash
./server
```

You should see output similar to:

```
Server listening on port 8080
```

The server is now waiting for an incoming connection from the client.

### Terminal 3: Run the Client

In a **third terminal window**, run your client application.

```bash
./client
```

You should see output similar to:

```
Server says: Hello from server
Server says: Another message from server
```

The client will connect, send data, receive data, and then close the connection.

### Observe `tcpdump` Output

As soon as the client connects and interacts with the server, you will see a flood of detailed packet information in your first `tcpdump` terminal.

**After the client finishes, you can press `Ctrl+C` in the `tcpdump` terminal to stop the capture.**

## Understanding the `tcpdump` Output

The `tcpdump` command: `sudo tcpdump -i lo tcp port 8080 -vvv -X` breaks down as follows:

  * `sudo`: Runs the command with superuser privileges, which is required to capture network traffic.
  * `tcpdump`: The command-line packet analyzer.
  * `-i lo`: Specifies the network interface to listen on. `lo` refers to the **loopback interface** (127.0.0.1). This is where your client and server communicate since they are both running on the same machine and connecting to `localhost`.
  * `tcp port 8080`: This is the **filter**. It tells `tcpdump` to only capture packets that use the TCP protocol and have either a source or destination port of `8080`.
  * `-vvv`: Increases the **verbosity** level. You'll get more detailed information about each packet, including TCP options.
  * `-X`: Dumps the packet's contents in both **hexadecimal and ASCII** format. This is extremely useful for seeing the actual data being sent (like "Hello from client").

### What to Look for in `tcpdump` Output:

#### 1\. The TCP 3-Way Handshake (Connection Establishment)

You will see three packets at the beginning of the communication:

  * **SYN (Synchronization):**

      * Sent by the client (`localhost.CLIENT_PORT > localhost.8080`).
      * `Flags [S]`: Indicates a SYN packet.
      * `seq X`: The client's initial sequence number.
      * This packet asks to establish a connection.

  * **SYN-ACK (Synchronization-Acknowledgement):**

      * Sent by the server (`localhost.8080 > localhost.CLIENT_PORT`).
      * `Flags [S.]`: Indicates a SYN and ACK packet.
      * `seq Y`: The server's initial sequence number.
      * `ack X+1`: Acknowledges the client's SYN, expecting the next sequence number from the client to be `X+1`.
      * This packet accepts the connection and asks the client to acknowledge.

  * **ACK (Acknowledgement):**

      * Sent by the client (`localhost.CLIENT_PORT > localhost.8080`).
      * `Flags [.]`: Indicates an ACK packet.
      * `seq X+1`: The client's next sequence number.
      * `ack Y+1`: Acknowledges the server's SYN-ACK, expecting the next sequence number from the server to be `Y+1`.
      * This packet completes the handshake, and the connection is now `ESTABLISHED`.

#### 2\. Data Transfer (PSH-ACK)

You will see packets where actual application data is transmitted:

  * **Client sending "Hello from client":**

      * Sent by the client (`localhost.CLIENT_PORT > localhost.8080`).
      * `Flags [P.]`: Indicates a PUSH and ACK. The `P` means the data should be pushed immediately to the application.
      * `length Z`: The number of bytes of application data.
      * Look at the hex dump (`-X` output) for the ASCII representation of "Hello from client".

  * **Server acknowledging client data:**

      * Sent by the server (`localhost.8080 > localhost.CLIENT_PORT`).
      * `Flags [.]`: An ACK packet.
      * `ack (client_seq_num + data_length)`: The server acknowledges the client's data.

  * **Server sending "Hello from server" / "Another message from server":**

      * Sent by the server (`localhost.8080 > localhost.CLIENT_PORT`).
      * `Flags [P.]`: PUSH and ACK.
      * Look at the hex dump for the ASCII representation of the server's messages.

  * **Client acknowledging server data:**

      * Sent by the client (`localhost.CLIENT_PORT > localhost.8080`).
      * `Flags [.]`: An ACK packet.
      * `ack (server_seq_num + data_length)`: The client acknowledges the server's data.

#### 3\. The TCP 4-Way Handshake (Connection Termination)

You will see four packets (or sometimes three, if a FIN and ACK are combined) at the end:

  * **FIN-ACK (Client Initiates Close):**

      * Sent by the client (`localhost.CLIENT_PORT > localhost.8080`).
      * `Flags [F.]`: Indicates a FIN and ACK. The client is saying it has no more data to send and wants to close its side of the connection.
      * The `FIN` flag consumes one sequence number.

  * **ACK (Server Acknowledges Client FIN):**

      * Sent by the server (`localhost.8080 > localhost.CLIENT_PORT`).
      * `Flags [.]`: An ACK packet.
      * `ack (client_FIN_seq_num + 1)`: Acknowledges the client's FIN. At this point, the server can still send data if it needs to, even though the client has initiated its close.

  * **FIN-ACK (Server Initiates Close):**

      * Sent by the server (`localhost.8080 > localhost.CLIENT_PORT`).
      * `Flags [F.]`: Indicates a FIN and ACK. The server is now saying it also has no more data and wants to close its side of the connection.

  * **ACK (Client Acknowledges Server FIN):**

      * Sent by the client (`localhost.CLIENT_PORT > localhost.8080`).
      * `Flags [.]`: An ACK packet.
      * `ack (server_FIN_seq_num + 1)`: Acknowledges the server's FIN. After sending this, the client enters a `TIME_WAIT` state for a brief period to ensure all packets are delivered and then closes the connection.
