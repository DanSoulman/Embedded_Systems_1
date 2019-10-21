/*
    C ECHO client example using sockets
*/
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
 
int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected!\n X= No Traffic N= North Traffic E= East Traffic B= Both Traffic\n");
    int first =1;
    //keep communicating with server
    while(1)
    {
        if (!first){
        	printf("Enter message : ");
        	scanf("%s" , message);
         
        	//Send some data
        	if( send(sock , message , strlen(message) , 0) < 0)
        	{
            	puts("Send failed");
            	return 1;
        	}
        }
	first =0;
        int len;
 
        //Receive a reply from the server
        if( (len=recv(sock , server_reply , 2000 , 0)) < 0)
        {
            puts("recv failed");
            break;
        }
	if (len >0){
        	server_reply[len] =0; 
        	printf("Server reply : %s",server_reply,"\n");
        }
    }
     
    close(sock);
    return 0;
}
