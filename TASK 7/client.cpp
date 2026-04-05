#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Error creating socket.\n";
        return 1;
    }

    // 2. Set server information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

    // 127.0.0.1 means same computer
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // 3. Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Connection failed.\n";
        return 1;
    }

    cout << "Connected to server.\n";

    while (true) {
        // 4. Client sends message
        string message;
        cout << "You: ";
        getline(cin, message);

        send(sock, message.c_str(), message.length(), 0);

        if (message == "bye") {
            cout << "Chat ended.\n";
            break;
        }

        // 5. Receive reply from server
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sock, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            cout << "Server disconnected.\n";
            break;
        }

        cout << "Server: " << buffer << endl;

        if (string(buffer) == "bye") {
            cout << "Chat ended.\n";
            break;
        }
    }

    close(sock);

    return 0;
}