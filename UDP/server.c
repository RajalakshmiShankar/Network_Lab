#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
int main(void)
{
int socket_desc;
struct sockaddr_in server_addr,
client_addr;
char server_message[2000],
client_message[2000];
int client_struct_length=
sizeof(client_addr);
//Clean buffers:
memset(server_message, '\0',
sizeof(server_message));
memset(client_message, '\0',
sizeof(client_message));
//Create UDP socket:
socket_desc = socket(AF_INET,
SOCK_DGRAM, IPPROTO_UDP);
if(socket_desc < 0){
printf("Error while creating
socket\n");
return -1;
}
printf("Socket created
successfully\n");
//Set port and IP:
server_addr.sin_family= AF_INET;
server_addr.sin_port=
htons(2000);
server_addr.sin_addr.s_addr=
inet_addr("127.0.0.1");
//Bind to the set port and IP:
if(bind(socket_desc, (struct
sockaddr*)&server_addr,sizeof(ser
ver_addr)) < 0){
printf("Couldn't bind to the port\n");
return -1;
}
printf("Done with binding........\n");
printf("Listening for incoming
messages......\n\n");
//Recieve client's message:
if(recvfrom(socket_desc,
    client_message,
    sizeof(client_message), 0, (struct
    sock_addr*)&client_addr,&client_s
    truct_length)<0){
    printf("Couldn't recieve message \n");
    return -1;
    }
    printf("Recieved message from IP:
    %d and port: %i
    \n",inet_ntoa(client_addr.sin_addr)
    ,ntohs(client_addr.sin_port));
    printf("Msg from client: %s \n",
    client_message);
    //Respond to Client:
    strcpy(server_message,
    client_message);
    if(sendto(socket_desc,
    server_message,
    strlen(server_message),0,(struct
    sockaddr*)&client_addr,client_stru
    ct_length) <0){
    printf("Can't send \n");
    return -1;
    }

    close(socket_desc);
    return 0;
}