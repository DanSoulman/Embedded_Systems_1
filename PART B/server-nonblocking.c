/*
    C socket server example, handles multiple clients using threads
*/

#include <stdio.h>
#include <string.h> //strlen
#include <sys/ioctl.h>
#include <stdlib.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h>   //for threading , link with lpthread

// Linked data structure

struct State
{
    unsigned long Out;
    unsigned long Time;
    unsigned long Next[4];
};

typedef const struct State STyp;

#define goN 0
#define waitN 1
#define goE 2
#define waitE 3

STyp FSM[4] = {
    {0x21, 3000, {goN, waitN, goN, waitN}},
    {0x22, 500, {goE, goE, goE, goE}},
    {0x0C, 3000, {goE, goE, waitE, waitE}},
    {0x14, 500, {goN, goN, goN, goN}}};

unsigned long S; // index to the current state
unsigned long Input;

//the thread function
void *connection_handler(void *);

int main(int argc, char *argv[])
{
    int socket_desc, client_sock, rc, c, *new_sock;
    struct sockaddr_in server, client;
    int on = 1;
    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        puts("Could not create socket");
    }
    puts("Socket created");
    /*************************************************************/
    /* Allow socket descriptor to be reuseable                   */
    /*************************************************************/
    rc = setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR,
                    (char *)&on, sizeof(on));
    if (rc < 0)
    {
        perror("setsockopt() failed");
        close(socket_desc);
        exit(-1);
    }

    /*************************************************************/
    /* Set socket to be nonblocking. All of the sockets for    */
    /* the incoming connections will also be nonblocking since  */
    /* they will inherit that state from the listening socket.   */
    /*************************************************************/
    rc = ioctl(socket_desc, FIONBIO, (char *)&on);
    if (rc < 0)
    {
        perror("ioctl() failed");
        close(socket_desc);
        exit(-1);
    }
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    //Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc, 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    while (1)
    {

        if ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)) >= 0)
        {
            puts("Connection accepted");

            pthread_t sniffer_thread;
            new_sock = malloc(sizeof(int));
            *new_sock = client_sock;

            if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0)
            {
                perror("could not create thread");
                return 1;
            }

            //Now join the thread , so that we dont terminate before the thread
            //pthread_join( sniffer_thread , NULL);
            puts("Handler assigned");
        }
    }
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }

    return 0;
}

/*
 * This will handle connection for each client
 * */
char client_message[2000];
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int *)socket_desc;
    int read_size;
    char *message;

    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";

    write(sock, message, strlen(message));

    message = "Now type something and i shall repeat what you type \n";

    write(sock, message, strlen(message));

    S = goN;
    int LIGHT;
    //Receive a message from client
    while ((read_size = recv(sock, client_message, 2000, 0)) > 0)
    {
        // sets lights
        LIGHT = FSM[S].Out;
        if (LIGHT & 1)
        {
            puts("North Green");
        }
        if (LIGHT & 2)
        {
            puts("North Yellow");
        }
        if (LIGHT & 4)
        {
            puts("North Red");
        }
        if (LIGHT & 8)
        {
            puts("East Green");
        }
        if (LIGHT & 16)
        {
            puts("East Yellow");
        }
        if (LIGHT & 32)
        {
            puts("East Red");
        }

        //Messages the client can pass in to
        if (client_message[0] == 'X')
        {
            puts("No Traffic\n");
            Input = 0; // read sensors
        }
        else if (client_message[0] == 'E')
        {
            puts("East Traffic\n");
            Input = 1; // read sensors
        }
        else if (client_message[0] == 'N')
        {
            puts("North Traffic\n");
            Input = 2; // read sensors
        }
        else if (client_message[0] == 'B')
        {
            puts("Both Traffic\n");
            Input = 3; // read sensors
        }

        S = FSM[S].Next[Input]; //Transitions to next state
        //Send the message back to client
        write(sock, client_message, strlen(client_message));
    }

    if (read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if (read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}
