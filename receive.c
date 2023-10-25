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
#include "read.h"
#include "write.h"
#include "transmit.h"
#include "receive.h"


static int SocketFD;
static struct addrinfo Hints, *ServInfo, *Travis;
static char* UsefulPort;
static List* LList;
static pthread_t ReceiverThread;

int AddressVal;
int BindVal;
int Bytes;
char Buf[256];
char* Message;
struct sockaddr_in TheirAddr;socklen_t AddrLen;


// <summary> This method sets up the hint conditions to create a GetAddress number
struct addrinfo RSetupHints(struct addrinfo HintsArg)
{
    memset(&HintsArg, 0, sizeof(HintsArg));
    HintsArg.ai_family = AF_INET;
    HintsArg.ai_socktype = SOCK_DGRAM;
    HintsArg.ai_flags = AI_PASSIVE;
    
    return HintsArg;
}


// <summary> Loops through potential sockets until one hits
int RSetupSocket(int Fd)
{

  Travis = ServInfo;
  while (Travis != NULL) {
      Fd = socket(Travis->ai_family, Travis->ai_socktype, Travis->ai_protocol);
  
      if (Fd == -1) {
          printf("Error: socket failed to create");
          Travis = Travis->ai_next;
          continue;
      }
  
      BindVal = bind(Fd, Travis->ai_addr, Travis->ai_addrlen);
  
      if (BindVal == -1) {
          close(SocketFD);
          printf("Error: socket could not bind");
          Travis = Travis->ai_next;
          continue;
      }
      break;
  }
  
  return Fd;
}


// <summary> Establish bytes to potentially receive
int RSetBytes(int ByteArg)
{
  memset(&Buf, 0, 256);

  AddrLen = sizeof(TheirAddr);
  ByteArg = recvfrom(SocketFD, Buf, 256, 0, (struct sockaddr *)&TheirAddr, &AddrLen);

  return ByteArg;
}

int containsSequence(const char *str) {
    int length = strlen(str);

    for (int i = 0; i < length - 1; i++) {
        if (str[i] == '!' && str[i + 1] == '\n') {
            return 1;
        }
    }

    return 0;
}


// <summary> constantly input messages to list, until either enter is pressed or ! is inputted
void* RLoadMessages()
{
        while (1)
        {
            Bytes = RSetBytes(Bytes);

            if (Bytes == -1)
            {
                printf("Error: Byte error");
                exit(-1);
            }

            Message = (char *)malloc(sizeof(char) * (Bytes + 1));
            strncpy(Message, Buf, Bytes);
            Message[Bytes] = '\0';

            int EnqueueVal = List_input(LList, Message);
            if (EnqueueVal == -1)
            {
                printf("Error: buffer overloaded");
            }

            if(!strcmp(Message, "!\n") && strlen(Message)==2)
            {
                SignalWriter();
                CancelRead();
                SignalTransmit();
                exit(1);
            }

            if (Buf[Bytes - 1] == '\n')
            {
                break;
            }
        }
    SignalWriter();
    return NULL;
}

// <summary> Establishes Socket and load messages
static void* ReceiverLoad(void* unused)
{

    Hints = RSetupHints(Hints);


    if ((AddressVal = getaddrinfo(NULL, UsefulPort, &Hints, &ServInfo)) != 0)
    {
        printf("Error: GAI is not created");
        exit(-1);
    }

    SocketFD = RSetupSocket(SocketFD);

    if (!Travis)
    {
        printf("Error: socket could not bind");
        exit(-1);
    }

    freeaddrinfo(ServInfo);

    while (1)
    {
        RLoadMessages();
    }
    return NULL;
}

// <summary> Constructor for receiver
void SetupReceiver(char* PortArg, List* ListArg)
{
    UsefulPort = PortArg;
    LList = ListArg;

    int RectVal = pthread_create(&ReceiverThread, NULL, ReceiverLoad, NULL);
    if (RectVal != 0)
    {
        exit(-1);
    }
}

void CancelReceiver()
{
    pthread_cancel(ReceiverThread);
}

// <summary> Destructor for receiver
void CloseReceiver()
{
    freeaddrinfo(ServInfo);
    close(SocketFD);
    pthread_join(ReceiverThread, NULL);
}
