// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>

#include <cstdlib>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <sstream>

#include <unordered_map>
#include <vector>

#define PORT 8080
#define SEQ 0
#define SUM 1

int CODE_TYPE;
std::unordered_map<std::string, int> word_mapping;
std::unordered_map<int, std::string> reverse_mapping;
int current = 1;

bool is_character(char c)
{
    if (c >= 'a' && c <= 'z') {
        return true;
    }
    if (c >= 'A' && c <= 'Z') {
        return true;
    }
    if (c >= '0' && c <= '9') {
        return true;
    }

    return false;
}

std::string encoding_to_string(std::vector<int> encodings) 
{
    std::string message;
    for(int i = 0; i < encodings.size(); i++) {
        std::stringstream oss;
        oss << "0x" << std::hex << encodings[i];
        message.append(oss.str());
        message.append(std::string(" "));
        oss.str("");
    }
    return message;
}


std::vector<std::string> split_message(std::string message)
{
    std::vector<std::string> words;
    std::string temp;
    for(int i = 0; i < message.length(); i++) {
        if(is_character(message[i])) {
            temp.append(std::string(1, message[i]));
        }
        else {
            if(temp.length() > 0) {
                words.push_back(temp);
                temp.clear();
            }
        }
    }
    if (temp.length() > 0) {
        words.push_back(temp);
        temp.clear();
    }

    return words;
}


int encode_word(std::string word)
{
    if (CODE_TYPE == SEQ) {
        if(word_mapping.count(word) > 0) {
            return word_mapping[word];
        }
        else {
            std::pair<std::string, int> element (word, current);
            std::pair<int, std::string> reverse(current, word);
            word_mapping.insert(element);
            reverse_mapping.insert(reverse);
            current++;
            return word_mapping[word];
        }
    }
    else if (CODE_TYPE == SUM) {
        int sum = 0;
        for(int i = 0; i < word.length(); i++) {
            sum += (int) word[i];
        }
        return sum;
    }
    else {
        exit(0);
    }
}


std::string decode_word(int code) {

    if(CODE_TYPE == SEQ) {
        return reverse_mapping[code];
    }
    else if(CODE_TYPE == SUM) {
        return std::string("test");
    }
}

std::string process_message(std::string message)
{
    bool encode = true;
    std::string response;

    if (message[0] == '0') {
        if (message[1] == 'x') {
            encode = false;
        }
    }
    std::vector<std::string> words = split_message(message);

    if (encode) {
        std::vector<int> encodings;
        for (int i = 0; i < words.size(); i++)
        {
            encodings.push_back(encode_word(words[i]));
        }
        response = encoding_to_string(encodings);
    }
    else {
        for(int i = 0; i < words.size(); i++) {
            int en = std::stoi(words[i].erase(0,2));
            response.append(decode_word(en));
            response.append(" ");
        }
    }

    response.append("\0");

    return response;
}


void server_loop(int port_num)
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_num);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    char buffer[257] = {0};
    while(1)
    {
        
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        std::fill_n(buffer, 257, '\0');
        int n = recv(new_socket, buffer, 256, 0);

        std::string message = std::string(buffer);
        std::string response = process_message(message);

        send(new_socket , response.c_str(), response.length(), 0);
        response.clear();
        close(new_socket);
    }
}
int main(int argc, char const *argv[]) 
{
    if(argc < 3) {
        std::cout << "server PORT ENCODE_TYPE\nEncoding types:\n 0: sequence\n 1: sum\n";
        exit(0);
    }
    int port_num = atoi(argv[1]);
    CODE_TYPE = atoi(argv[2]);

    server_loop(port_num);
}
