#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string>
#include <sstream>
#include <iostream>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

std::string get_entire_response(int sockfd)
{
    std::string page;
    int BUFFER_SIZE = 256;
    char buffer[BUFFER_SIZE];
    bool keep_reading = true;
    int n;

    while(keep_reading)
    {
        std::fill_n(buffer, BUFFER_SIZE, '\0');
        n = recv(sockfd, buffer, BUFFER_SIZE, 0);

        if(n <= 0)
        {
            keep_reading = false;
        }
        
        page.append(buffer);
    }
    close(sockfd);

    return page;
}

std::string send_message(std::string message, std::string hostname, int port)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int bytes_read;

    portno = 8081;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(hostname.c_str());
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    n = write(sockfd, message.c_str(), message.size());
    if (n < 0)
        error("ERROR writing to socket");

    char buffer[2048];
    n = recv(sockfd, buffer, 2047, 0);
    return std::string(buffer);
}

int main(int argc, char *argv[])
{

    if(argc < 4) {
        std::cout << "client HOSTNAME PORT MESSAGE\n";
        exit(0);
    }
    std::string message;
    if(argc > 1) {
        for(int i = 3; i < argc; i++) {
            message.append(argv[i]);
            message.append(" ");
        }
        message.pop_back();
    }
    int port = atoi(argv[2]);
    std::cout << send_message(message, argv[1], port) << "\n";


    return 0;
}

