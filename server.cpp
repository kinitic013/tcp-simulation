#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address{};
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Reuse address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Set address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Accept from any interface
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    // Accept loop
    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept failed");
            continue;
        }

        std::cout << "Client connected" << std::endl;

        ssize_t valread = read(new_socket, buffer, sizeof(buffer));
        if (valread > 0) {
            std::cout << "Message from client: " << buffer << std::endl;

            send(new_socket, "Hello from server", 17, 0);
            std::cout << "Reply sent\n";

            memset(buffer, 0, sizeof(buffer));  // Clear buffer

            send(new_socket, "Another message from server", 27, 0);
        } else {
            std::cout << "Read failed or client disconnected early\n";
        }

        close(new_socket);  // Important: Close client socket after response
    }

    close(server_fd);
    return 0;
}
