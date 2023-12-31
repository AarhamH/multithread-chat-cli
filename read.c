#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "list.h"
#include "read.h"
#include "receive.h"
#include "transmit.h"

static List* LList;
static pthread_t Reader;
char* Message;
char BufferStorage[256];
int Bytes;

// <summary> Initializes space to read
int RdSetBytes(int BytesArg)
{
    memset(BufferStorage, 0, 256);
    BytesArg = read(0, BufferStorage, 256);
    return BytesArg;
}

// <summary> Allocate space based on the bytes receieved
char* RdSetMessageBuffer(char* MessageArg, int BytesArg)
{
  MessageArg = (char*)malloc(sizeof(char) * (BytesArg + 1));
  strncpy(MessageArg, BufferStorage, BytesArg);
  MessageArg[BytesArg] = '\0';

  return MessageArg;
}


// <summary> Unloads all of the messages until enter is pressed or ! is inputted
static void* ReadUnload() {
    while (1) {

        while (1) {

            Bytes = RdSetBytes(Bytes);
            if (Bytes == -1) {
                printf("Error: not enough bytes allocated");
                exit(-1);
            }

            Message = RdSetMessageBuffer(Message,Bytes);

            int Res = List_input(LList,Message);
            if (Res == -1) {
                printf("Error: Message cannot be read");
            }

            if (!strcmp(Message, "!\n") && strlen(Message)==2)
            {
              SignalTransmit();
              CancelReceiver();
              CancelTransmit();
              exit(1);
            }

            if (BufferStorage[Bytes - 1] == '\n') {
                SignalTransmit();
                break;
            }
        }
    }
    return NULL;
}


// <summary> Reader constructor
void SetupRead(List* ListArg) {
    LList = ListArg;

    int ReadingThread =  pthread_create(&Reader, NULL, ReadUnload, NULL);
    if (ReadingThread != 0) {
        printf("Error: Threads not created properly");
        exit(-1);
    }
}


void CancelRead() 
{
    pthread_cancel(Reader);
}


// <summary> Reader Destructor
void CloseRead() 
{
    pthread_join(Reader, NULL);
}
