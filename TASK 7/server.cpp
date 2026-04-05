#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

int main() {
    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_size = sizeof(client_addr);

    char buffer[1024];

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Error creating socket.\n";
        return 1;
    }

    // 2. Set server address info
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;   // accept connections from any IP
    server_addr.sin_port = htons(8080);         // port number

    // 3. Bind socket to IP and port
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Bind failed.\n";
        return 1;
    }

    // 4. Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        cerr << "Listen failed.\n";
        return 1;
    }

    cout << "Server is waiting for connection on port 8080...\n";

    // 5. Accept client connection
    client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_size);
    if (client_socket < 0) {
        cerr << "Accept failed.\n";
        return 1;
    }

    cout << "Client connected.\n";

    while (true) {
        // 6. Receive message from client
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            cout << "Client disconnected.\n";
            break;
        }

        cout << "Client: " << buffer << endl;

        // 7. If client says bye, end chat
        if (string(buffer) == "bye") {
            cout << "Chat ended.\n";
            break;
        }

        // 8. Server sends reply
        string message;
        cout << "You: ";
        getline(cin, message);

        send(client_socket, message.c_str(), message.length(), 0);

        if (message == "bye") {
            cout << "Chat ended.\n";
            break;
        }
    }

    close(client_socket);
    close(server_fd);

    return 0;
}