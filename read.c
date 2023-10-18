#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "list.h"
#include "read.h"
#include "recieve.h"
#include "transmit.h"

static List* LList;
static pthread_t ReaderThread;
char* Message;
char BufferStorage[256];
int Bytes;


int SetBytes(int BytesArg)
{
    memset(BufferStorage, 0, 256);
    BytesArg = read(0, BufferStorage, 256);
    return BytesArg;
}

char* SetMessageBuffer(char* MessageArg, int BytesArg)
{
  MessageArg = (char*)malloc(sizeof(char) * (BytesArg + 1));
  strncpy(MessageArg, BufferStorage, BytesArg);
  MessageArg[BytesArg] = '\0';

  return MessageArg;
}

static void* ReadUnload() {
    while (1) {

        int Iteration = 0;

        while (1) {
            Iteration++;

            Bytes = SetBytes(Bytes);
            if (Bytes == -1) {
                printf("Error: not enough bytes allocated");
                exit(-1);
            }

            Message = SetMessageBuffer(Message,Bytes);

            int Res = List_input(LList,Message);
            if (Res == -1) {
                printf("Error: Message cannot be read");
            }

            if (memcmp(Message, "!\n", 2) == 0) {
                if (Iteration == 1) {
                    senderSignaller();
                    receiverCancel();
                    senderCancel();
                    return NULL;
                }
            }

            if (BufferStorage[Bytes - 1] == '\n' || Iteration == 100) {
                senderSignaller();
                break;
            }
        }
    }
    return NULL;
}

void readerInit(List* ListArg) {
    LList = ListArg;

    int ReadingThread =  pthread_create(&ReaderThread, NULL, ReadUnload, NULL);
    if (ReadingThread != 0) {
        printf("Error: Threads not created properly");
        exit(-1);
    }
}

void readerCancel() 
{
    pthread_cancel(ReaderThread);
}

void readerShutdown() 
{
    pthread_join(ReaderThread, NULL);
}
