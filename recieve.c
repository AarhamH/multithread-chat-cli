#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 22110
#define MAX_LENGTH 1024

void* receiver_thread(void* arg) {
    char messageBuffer[MAX_LENGTH];

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    int mysocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (mysocket == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(mysocket, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("Could not bind socket");
        exit(EXIT_FAILURE);
    }

    printf("Receiver thread started\n");
    printf("To connect do: netcat -u 127.0.0.1 %d\n", PORT);

    while (1) {
        struct sockaddr_in messageFrom;
        unsigned int messageLen = sizeof(messageFrom);
        int bytesOfMessage = recvfrom(mysocket, messageBuffer, MAX_LENGTH, 0, (struct sockaddr*)&messageFrom, &messageLen);

        if (bytesOfMessage == -1) {
            perror("Could not receive message");
            continue;
        }

        int terminateNull = (bytesOfMessage < MAX_LENGTH) ? bytesOfMessage : MAX_LENGTH - 1;
        messageBuffer[terminateNull] = 0;

        printf("Message received successfully by: %d bytes:\n\n%s\n", bytesOfMessage, messageBuffer);
    }

    close(mysocket);
    return NULL;
}

int main() {
    pthread_t receiverThread;
    if (pthread_create(&receiverThread, NULL, receiver_thread, NULL) != 0) {
        perror("Could not create thread");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(receiverThread, NULL) != 0) {
        perror("Could not join thread");
        exit(EXIT_FAILURE);
    }

    return 0;
}