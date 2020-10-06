#include <iostream>
#include <sys/socket.h>
#include <cstdlib>
#include <cstdio>
#include <netdb.h>
#include <sys/types.h>
#include <cstring>
#include <netinet//in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

using namespace std;

int main() {


    //Declare all the variables needed to create a socket
    int client, server;
    int buffersize = 4096;
    char buffer[buffersize], command[100], filename[20];
    struct stat obj;
    int c, k, size, file_description;
    struct sockaddr_in server_address, from_address;
    char *f;
    socklen_t fromLength;

    //Initialize the socket
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        cout << "Error establishing connection." << endl;
        exit(1);
    }

    //Create the socket
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(54000);
    server_address.sin_addr.s_addr = INADDR_ANY;
    memset(server_address.sin_zero, '\0', sizeof(server_address.sin_zero));

    //bind the socket
    k = bind(server, (struct sockaddr *) &server_address, sizeof(server_address));
    if (k < 0) {
        cout << "Error binding socket" << endl;
        exit(1);
    }

    //listen for socket connection
    k = listen(server, 5);
    if (k < 0) {
        cout << "Listen failed" << endl;
        exit(1);
    }

    //Accept the connection
    client = accept(server, (struct sockaddr *) &from_address, &fromLength);
    if (client < 0) {
        cout << "Error accepting connection" << endl;
        exit(1);

    }

    //While loop to keep the connection open
    int i = 1;
    while (1) {
        recv(client, buffer, sizeof(buffer), 0);
        sscanf(buffer, "%s", command);
        if (!strcmp(command, "catalog")) {
            system("ls > output1.txt");
            i = 0;
            stat("output1.txt", &obj);
            size = obj.st_size;
            send(client, &size, sizeof(int), 0);
            file_description = open("output1.txt", O_RDONLY, 777);
            sendfile(client, file_description, nullptr, size);
        } else if (!strcmp(command, "download")) {
            sscanf(buffer, "%s%s", filename, filename);
            stat(filename, &obj);
            file_description = open(filename, O_RDONLY);
            size = obj.st_size;
            if (file_description == -1) {
                size = 0;
            }
            send(client, &size, sizeof(int), 0);
            if (size) {
                sendfile(client, file_description, nullptr, size);
            }
        } else if (!strcmp(command, "upload")) {

            recv(client, buffer, sizeof(buffer), 0);
            cout << "Preparing to receive file:  " << buffer << endl;

            sscanf(buffer, "%s", filename);
            recv(client, &size, sizeof(int), 0);
            i = 1;
            while (1) {
                file_description = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                if (file_description == -1) {
                    sprintf(filename + strlen(filename), "%d", i);
                } else {
                    break;
                }
            }
            f = (char *) malloc(size);
            cout << "The expected files size is: " << size << " bytes" << endl;
            recv(client, f, size, 0);
            c = write(file_description, f, size);
            if(c < 0){
                cout << "Error uploading file to server" << endl;
                break;
            }
            cout << "Successfully received file: " << filename << endl << "Bytes received: " << c << endl;
            close(file_description);
            send(client, &c, sizeof(int), 0);
        } else if (!strcmp(command, "bye")) {
            printf("Server is shutting down...\n");
            i = 1;
            send(client, &i, sizeof(int), 0);
            close(client);
            close(server);
            exit(0);
        }
    }

    return 0;
}














