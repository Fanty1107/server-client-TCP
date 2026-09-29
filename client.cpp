#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Erro ao criar o socket\n";
        return 1;
    }

    sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        std::cerr << "Endereço inválido\n";
        close(clientSocket);
        return 1;
    }

    int conn = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (conn < 0) {
        std::cerr << "Não foi possível conectar ao servidor. Verifique se o servidor está rodando.\n";
        close(clientSocket);
        return 1;
    }
    std::string input;
    std::cout << "Enter your message to server: ";
    std::getline(std::cin, input);

    ssize_t s = send(clientSocket, input.c_str(), input.size(), 0);
    if (s < 0) {
        std::cerr << "Erro ao enviar mensagem\n";
        close(clientSocket);
        return 1;
      }
    char buffer[1024];
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "Resposta do servidor: " << buffer;
    } else if (bytesReceived == 0) {
        std::cout << "Servidor fechou a conexão antes de responder.\n";
    } else {
        std::cerr << "Erro ao receber resposta do servidor\n";
    }

    close(clientSocket);
    return 0;
}
