#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "read.h"
#include "write.h"
#include "receive.h"
#include "transmit.h"

// Launches the s-talk program
int main (int argc, char * argv[])
{
    printf("Hello User! To use s-talk, begin typing.\n ");

    if (argc!=4)
    {
        printf("Invalid Credentials. Try Again\n");
        return -1;
    }

    char* myPort = argv[1];
    char* theirHostname = argv[2];
    char* theirPort = argv[3];

    // Creates a shared list
    List* list = List_create();

    SetupRead(list);
    SetupTransmit(theirHostname, theirPort, list);
    SetupReceiver(myPort, list);
    SetupWriter(theirHostname,list);

    CloseRead();
    CloseTransmit();
    CloseReceiver();
    CloseWriter();

    List_free(list, free);

    return 0;
}