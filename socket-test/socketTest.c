//socket 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 22110
#define MAX_LENGTH 1024
char messageBuffer[MAX_LENGTH]; // buffer to store the message data

int main(int argCount,char* args[]){

    printf("Hello!\n");
    printf("to connect do: netcat -u 127.0.0.1 %d\n", PORT);



    // setting up socket -- brian had //
    struct sockaddr_in server; 
    memset(&server,0, sizeof(server)); // clear the socket
    server.sin_family = AF_INET; 
    server.sin_addr.s_addr = htonl(INADDR_ANY); // in address any, willing to work on any local port, 
                            //dont care about port sending to
    server.sin_port = htons(PORT); // port to connect to 


    // creating socket// 
    int mysocket = socket(PF_INET, SOCK_DGRAM, 0); 
        //before was AF  instead PF -- domain = AF-> allows communication and binding so it can talk to another port
        // type = DGRAM -> connectionless socket,for UDP, no need to maintain open connection

    // binding socket //
    int bindval= bind(mysocket, (struct sockaddr *)&server, sizeof(server));


    while(1){

    // recieve data// 
        struct sockaddr_in messageFrom; // lets us know who we got message from, brian had sinRemote
        unsigned int messageLen = sizeof(messageFrom); // the length
        int bytesOfMesssage = recvfrom(mysocket, messageBuffer, MAX_LENGTH, 0, (struct sockaddr *)&messageFrom,&messageLen); // main function to recieve

        int interminateNull = (bytesOfMesssage < MAX_LENGTH) ? bytesOfMesssage : MAX_LENGTH -1; 
        messageBuffer[interminateNull] = 0; 
        printf("message recived successfully by: %d bytes: \n\n %s\n", bytesOfMesssage,messageBuffer);
        if(messageBuffer == NULL){
            printf("repeat-->  %s\n", messageBuffer);
        }
        

    }


    //close socket// 

    close(mysocket); 

    
}

