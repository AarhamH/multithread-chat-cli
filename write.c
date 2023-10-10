#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "list.h"

static pthread_t Writer;
static pthread_mutex_t AvailableWriterMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t AvailableWriterCond = PTHREAD_COND_INITIALIZER;
static List* LList;
static char* Message;


// <summary> This method unloads all of the messages in the queue. The method waits for a signal based on the 
//           Cond; when it recieves the signal, writer unloads everything in the list
static void* WriterUnload(){

    while(1)
    {
        // Wait for signal before proceeding
        pthread_mutex_lock(&AvailableWriterMutex);
        {
            pthread_cond_wait(&AvailableWriterCond, &AvailableWriterMutex);
        }
        pthread_mutex_unlock(&AvailableWriterMutex);
        
        int Traverser = 0;
        
        // Unloads everything in LList
        while (1)
        {
            if (List_lockedCount(LList) == 0) break;
            
            Traverser ++;
            Message = List_output(LList);

            if(!Message)
            {
                printf("Error: Trimming did not work!.\n");
                break;
            }

            int writeVar = write(1, Message, strlen(Message)); // will put the message from first LL onto the screen
            if(writeVar == -1){
                exit(-1);
            }

            // If '!' is pressed, get out!
            if(!strcmp(Message, "!\n") && Traverser == 1)
            {
                free(Message);
                Message = NULL;
                return NULL;
            }

            free(Message);
            Message = NULL;

        }
    }
    return NULL;
}

// <summary> Sets up writer with a linked list and creates a thread with unload function
void SetupWriter(List* l){

    LList = l;

    int writingThread =  pthread_create(&Writer, NULL, WriterUnload, NULL);
    if(writingThread != 0){
        perror("write thread failed");
        exit(-1);
    }

}

// <summary> This method signals the writer that there are messages to be unloaded
void SignalWriter() {
    pthread_mutex_lock(&AvailableWriterMutex);
    pthread_cond_signal(&AvailableWriterCond);
    pthread_mutex_unlock(&AvailableWriterMutex);
}

// <summary> This method cancels the writer thread, stopping it from further operations
void CancelWriter() {
    pthread_cancel(Writer);
    pthread_join(Writer, NULL); 
    free(Message);
    Message = NULL;
}

// <summary> Destroys the final message and closes thread
void CloseWriter()
{
    free(Message);
    Message = NULL;

    pthread_join(Writer, NULL);
}



