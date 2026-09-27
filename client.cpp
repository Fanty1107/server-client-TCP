#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // sending connection request
    int conn = connect(clientSocket, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));
    if (conn < 0) {
      std::cerr << "Unable to connect to server make sure the server is up\n";
      return 1;
    }

    // sending data
    const char* message = "what tha dog doing";
    int s = send(clientSocket, message, strlen(message), 0);
    if (s < 0) {
      std::cerr << "error on send message\n"; 
    }

    // closing socket
    close(clientSocket);

    return 0;
}
