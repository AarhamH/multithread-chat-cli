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
#include <pthread.h>
#include "list.h"
#include "transmit.h"


// Static 
static int SocketEndPoint;
static struct addrinfo *ServInfo;
static char* Hostname;
static char* Port;
static List* LList;
static pthread_t SenderThread;
static char* Message;
struct addrinfo Hints, *ListTraverse;

// what
int Bytes;
int GetAddress;


// Mutual exclusions
static pthread_mutex_t SendAvailableCondMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t SendAvailableCond = PTHREAD_COND_INITIALIZER;


// <summary> This method sets up the hint conditions to create a GetAddress number
struct addrinfo TSetupHints(struct addrinfo HintsArg)
{
    memset(&HintsArg, 0 ,sizeof (HintsArg));
    HintsArg.ai_family = AF_INET;
    HintsArg.ai_socktype = SOCK_DGRAM;
    
    return HintsArg;
}


// <summary> This method finds an available socket and sets the endpoint. Returns -1 by default if none are found.
int TSetupSocket(int Fd)
{
    for (ListTraverse = ServInfo; ListTraverse != NULL; ListTraverse = ListTraverse->ai_next) {
        Fd = socket(ListTraverse->ai_family, ListTraverse->ai_socktype, ListTraverse->ai_protocol);

        if (Fd == -1) {
            printf("Error: socket did not set up or find empty");
            continue;
        }
        break;
    }
    return Fd;
}

// <summary> This method trims the messages one by one until none is left. Called by SetupUnload()
void* TUnloadMessages()
{
        int ListCount;
        
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) != 0) 
        {
            printf("Error: Wrong hostname");
            exit(-1);
        }

        while ((ListCount = List_lockedCount(LList)) != 0) 
        {

            // Getting message from list
            Message = List_output(LList);

            if (Message == NULL) {
                printf("Error: Message is not allocated well.");
                break;
            }

            // Sending
            Bytes = sendto(SocketEndPoint, Message, strlen(Message), 0, ListTraverse->ai_addr, ListTraverse->ai_addrlen);

            // Check for exit code
            // Ends the thread if exit code is passed in the first iteration of the read
            if (!strcmp(Message, "!\n") && strlen(Message)==2) {
                free(Message);
                Message = NULL;
                return NULL;
            }

            // De-allocating message
            free(Message);
            Message = NULL;

            // Error checking recvfrom
            if (Bytes == -1) {
                printf("Error: bytes are bad");
                exit(-1);
            }
        }
        return NULL;
}

// <summary> This method sets up a semaphore, if wait condition is lifted, calls UnloadMessages() to send all messages along socket
static void* TransmitUnload() {
    
    Hints = TSetupHints(Hints);

    if ((GetAddress = getaddrinfo(Hostname, Port, &Hints, &ServInfo)) != 0) {
        printf("Error: GAI Value bad");
        exit(-1);
    }

    SocketEndPoint = TSetupSocket(SocketEndPoint);

    if (ListTraverse == NULL) {
        printf("Error: Socket did not create");
        exit(-1);
    }

    while (1) {
        pthread_mutex_lock(&SendAvailableCondMutex);
        {
          pthread_cond_wait(&SendAvailableCond, &SendAvailableCondMutex);
        }
        pthread_mutex_unlock(&SendAvailableCondMutex);
        
        TUnloadMessages();

    }
    return NULL;
}

// <summary> This method is called by another thread to say that it is ready
void SignalTransmit() {
    pthread_mutex_lock(&SendAvailableCondMutex);
    {
      pthread_cond_signal(&SendAvailableCond);
    }
    pthread_mutex_unlock(&SendAvailableCondMutex);
}

// <summary> This method cancels sender
void CancelTransmit() {
    pthread_cancel(SenderThread);
}


// <summary> This method initializes sender
void SetupTransmit(char* HostnameArg, char* PortArg, List* ListArg) {
    Hostname = HostnameArg;
    Port = PortArg;
    LList = ListArg;

    int StVal = pthread_create(&SenderThread, NULL, TransmitUnload, NULL);
    if (StVal != 0) {
        printf("Error: transmitter did not create");
        exit(-1);
    }
}

// <summary> This method frees all allocated messages and closes the sender thread
void CloseTransmit() {
    free(Message);
    Message = NULL;

    freeaddrinfo(ServInfo);

    close(SocketEndPoint);

    pthread_join(SenderThread, NULL);
}
