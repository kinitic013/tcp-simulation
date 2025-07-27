#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr{};
    char buffer[1024] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cout << "Socket creation error\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection failed\n";
        return -1;
    }

    send(sock, "Hello from client", 18, 0);
    read(sock, buffer, 1024);
    std::cout << "Server says: " << buffer << std::endl;
    buffer[1024] = {0}; // Ensure null-termination
    read(sock, buffer, 1024);
    std::cout << "Server says: " << buffer << std::endl;
    close(sock);
    return 0;
}
