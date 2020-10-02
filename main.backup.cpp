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
    bool isExit;
    char server_message[256] = "You have reached Ryan's Server...";
    int j, k, size, filehandle;

    struct sockaddr_in server_address, from_address;
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
//    int yes = 1;
//    if(setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1){
//        perror("setsockopt");
//        exit(1);
//    }

    if (k < 0) {
        cout << "Error binding socket" << endl;
        exit(1);
    }

    k = listen(server, 5);
    if (k < 0) {
        cout << "Listen failed" << endl;
        exit(1);
    }



    //strcpy(buffer, "Welcome to Ryan's Server\n");
    //send(client, buffer, buffersize, 0);

    //recv(client, buffer, sizeof(buffer), 0);
    //sscanf(buffer, "%s", command);

    //Accept the connection
    client = accept(server, (struct sockaddr *) &from_address, &fromLength);
    if (client < 0) {

        cout << "Error accepting connection" << endl;
        exit(1);

    }
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
            filehandle = open("output1.txt", O_RDONLY, 777);
            sendfile(client, filehandle, nullptr, size);
        } else if (!strcmp(command, "download")) {
            sscanf(buffer, "%s%s", filename, filename);
            stat(filename, &obj);
            filehandle = open(filename, O_RDONLY);
            size = obj.st_size;
            if (filehandle == -1) {
                size = 0;
            }
            send(client, &size, sizeof(int), 0);
            if (size) {
                sendfile(client, filehandle, nullptr, size);
            }
        } else if (!strcmp(command, "upload")) {
            int c = 0, len;
            char *f;
            recv(client, buffer, sizeof(buffer), 0);
            sscanf(buffer, "%s", filename);
            recv(client, &size, sizeof(int), 0);
            i = 1;
            while (1) {
                filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                if (filehandle == -1) {
                    sprintf(filename + strlen(filename), "%d", i);
                } else {
                    break;
                }
            }
            f = (char *) malloc(size);
            recv(client, f, size, 0);
            c = write(filehandle, f, size);
            close(filehandle);
            send(client, &c, sizeof(int), 0);
        } else if (!strcmp(command, "bye")) {
            printf("File copy server is shutting down...\n");
            i = 1;
            send(client, &i, sizeof(int), 0);
            exit(0);
        }
    }

    return 0;
}



    /*
    while(client > 0){
        strcpy(buffer, "Server connected...\n");
        send(client, buffer, buffersize, 0);

        cout << "Connected with client." << endl;
        cout << "Enter 'Bye' to end connection" << endl;

        cout << "Client: ";
        do{
            recv(client, buffer, buffersize, 0);
            cout << "buffer" << " " ;
            if(*buffer == 'Bye'){
                *buffer = '*';
                isExit = true;
            }while (*buffer != '*');
        }

    }

    send(client_socket, server_message, sizeof(server_message), 0);
    char new_message[256] = "Please type in your command";

    send(client_socket, new_message, sizeof(new_message), 0);

    char command[256];
    client_socket = recv(client_socket, command, sizeof(command), 0);



    //char commandCompare[256] = "ls";
    //int compareStrings = strcmp(command, commandCompare);


    //if(compareStrings == 0){
    //    strcpy(command, "ls");
    //    system(command);
    //}else{
    //    printf("That command is invalid");
    //}



    //printf("The client sent: %s\n", command);
    */











