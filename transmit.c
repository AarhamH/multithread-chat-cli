#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


//AddressInfo structs and endpoints
static int SocketEndpointValue = -1;
static struct addrinfo Hints, *ServInfo, *ListTraverse;
static int AddressValue;
static int Bytes;
static char* ListMessage;


// <summary> Initializes an address info struct hint, which determines the type of socket we are seeking
//           The family and type are configured to support IPv4 and UDP respectively
struct addrinfo SetUpHints(struct addrinfo HintsArg)
{
    memset(&HintsArg, 0 ,sizeof (HintsArg));
    HintsArg.ai_family = AF_INET;
    HintsArg.ai_socktype = SOCK_DGRAM;
    return HintsArg;
}


// <summary> Sets up a potential socket. This does this by traversing through ServInfo's Linked List based
//           on its requirements (family, type, etc.). Returns socket identity value as an int
int SetUpSocket(int SocketArg)
{
    ListTraverse = ServInfo;
    while (ListTraverse != NULL)
    {
        SocketArg = socket(ListTraverse->ai_family, ListTraverse->ai_socktype, ListTraverse->ai_protocol);
    
        if (SocketArg == -1)
        {
            perror("sender: socket() error");
            ListTraverse = ListTraverse->ai_next; // Move to the next addrinfo structure
            continue;
        }
    
        break;
    }
    
    return SocketArg;
}

// <summary> Initializes all of the necessary components, such as hints, GAI, and Socket and creates and instance
//           of the sender. After setup, sender unloads (dequeues) all of the messages from the list arg
void SetupUDPSender(char* hostname, char* port, List* list)
{
    // Initializing Hints
    Hints = SetUpHints(Hints);

    if (AddressValue = getaddrinfo(hostname, port, &Hints, &ServInfo) != 0)
    {
        fprintf(stderr, "Error: %s\n", gai_strerror(AddressValue));
        return;
    }

    SocketEndpointValue = SetUpSocket(SocketEndpointValue);
    // Error checking if no sockets are binded
    if (ListTraverse == NULL)
    {
        fprintf(stderr, "Error: Failed to create socket in Sender");
        return;
    }
    
    // Loop to dequeue list every time and send it over via socket; checking if the bytes are valid
    while(1)
    {
        if(List_count(list) > 0)
        {
              ListMessage = (char*)List_trim(list);
    
              if((Bytes = sendto(SocketEndpointValue, ListMessage, strlen(ListMessage), 0, ListTraverse->ai_addr, ListTraverse->ai_addrlen)) == -1)
              {
                perror("Error: Unable to send message");
                return;
              }
        }

    }

    freeaddrinfo(ServInfo);
}


// <summary> Closes the socket file descriptor...
void CloseUDPSender()
{
    close(SocketEndpointValue);
}


// ---- TODO: write some tests to verify if -----
// 1.) The sender is reading real hostnames and port numbers
// 2.) If the sender is transmitting bytes
// 3.) The bytes are being sent to the correct location
// *4.) When testing is done, complete the header by defining SetupUDPSender and CloseUDPSender
int main() 
{
    // Sample input
    char* hostname = "asb9700u-j08"; // Replace with your target hostname
    char* port = "9999999999999"; // Replace with the target port
    List* messageList = List_create();
  
    
    // Sample messages
    List_insert_after(messageList, "Hello, world!");
    List_insert_after(messageList, "How are you?");
    List_insert_after(messageList, "Goodbye!");
    List_insert_after(messageList, "Yes!");
    List_insert_after(messageList, "No!");
    List_insert_after(messageList, "What!");
    
    // Initialize the sender
    SetupUDPSender(hostname, port, messageList);

    // Simulate sending messages for a few seconds
    sleep(5); // Adjust the duration as needed

    // Shutdown the sender
    CloseUDPSender();

    // Free the message list
    List_free(messageList, NULL);
    

    return 0;
}