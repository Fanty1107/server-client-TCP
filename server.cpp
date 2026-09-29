#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

int bindingSocket(int serverSocket, sockaddr_in serverAddr) {
    int berr = bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (berr < 0) {
        std::cerr << "error to bind\n";
        return 1;
    }
    return 0;
}

void sendingData(int socket, const std::string& data) {
    if (data.empty()) {
        std::cerr << "ERROR: data is empty\n";
        return;
    }

    ssize_t result = send(socket, data.c_str(), data.size(), 0);
    if (result < 0) {
        std::cerr << "error in sending data from server\n";
    }
}

std::string processMessageFromClient(const char *buffer) {
    if (buffer == nullptr) {
        return "";
    }
    std::string clean = buffer;
    while (!clean.empty() && (clean.back() == '\n' || clean.back() == '\r')) {
        clean.pop_back();
    }

    if (clean == "12345") {
        return "you enter the right password\n";
    } else {
        return "wrong password, access denied\n";
    }
}

int main() {
    // 1. Criar socket com validação
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "error creating socket\n";
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bindingSocket(serverSocket, serverAddress) != 0) {
        close(serverSocket);
        return 1;
    }
    if (listen(serverSocket, 100) < 0) {
        std::cerr << "error to listen\n";
        close(serverSocket);
        return 1;
    }

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket < 0) {
        std::cerr << "error to accept\n";
        close(serverSocket);
        return 1;
    }

    char buffer[1024];
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0'; // Garante o término de string
        std::cout << "Message from client: " << buffer << std::endl;

        std::string messageToClient = processMessageFromClient(buffer);
        sendingData(clientSocket, messageToClient);
    } else if (bytesReceived == 0) {
        std::cout << "Client disconnected before sending data\n";
    } else {
        std::cerr << "recv error\n";
    }

    close(clientSocket);
    close(serverSocket);

    return 0;
}
