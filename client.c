#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 1883
#define BUFFER_SIZE 1024

void mqtt_connect(int sock) {
    char buffer[BUFFER_SIZE];
    int length = 0;

    // Variable header
    buffer[length++] = 0x00;
    buffer[length++] = 0x04;
    buffer[length++] = 'M';
    buffer[length++] = 'Q';
    buffer[length++] = 'T';
    buffer[length++] = 'T';
    buffer[length++] = 0x04;
    buffer[length++] = 0x02;
    buffer[length++] = 0x00;
    buffer[length++] = 0x3C;

    // Payload
    buffer[length++] = 0x00;
    buffer[length++] = 0x06;
    buffer[length++] = 'c';
    buffer[length++] = 'l';
    buffer[length++] = 'i';
    buffer[length++] = 'e';
    buffer[length++] = 'n';
    buffer[length++] = 't';

    // Fixed header
    buffer[0] = 0x10;
    buffer[1] = length - 2;

    send(sock, buffer, length, 0);
}

void mqtt_publish(int sock, const char* topic, const char* message) {
    char buffer[BUFFER_SIZE];
    int length = 0;

    // Variable header
    buffer[length++] = 0x00;
    buffer[length++] = strlen(topic);
    memcpy(buffer + length, topic, strlen(topic));
    length += strlen(topic);

    // Payload
    memcpy(buffer + length, message, strlen(message));
    length += strlen(message);

    // Fixed header
    buffer[0] = 0x30;
    buffer[1] = length - 2;

    send(sock, buffer, length, 0);
}

int main() {
    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }

    mqtt_connect(sock);
    mqtt_publish(sock, "example/topic", "Hello, MQTT!");

    close(sock);
    return 0;
}