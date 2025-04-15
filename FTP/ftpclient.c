#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
int main(void)
{
    FILE *fp;
    int n,s;
    char name[100],rcvmsg[100],rcvg[100],fname[100];
    int socket_desc;
    struct sockaddr_in server_addr;
    
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_desc < 0){
        printf("Unable to Create Socket\n");
        return -1;
    }
    
    printf("Socket Created Successfully\n");
    
    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Send connection request to server:
    if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Unable to Connect\n");
        return -1;
    }
    printf("Connected with Server Successfully\n");
    
    // Get input from the user:
    printf("Enter filename ");
    scanf("%s",name);
    printf("Enter the new file name\t");
    scanf("%s",fname);
    fp=fopen(fname,"w");
    send(socket_desc,name,sizeof(name),0);
    while(1){
             s=recv(socket_desc,rcvg,100,0);
             rcvg[s]='\0';
             if(strcmp(rcvg,"error")==0)
             {
                 printf("File is not available\n");
                 exit(1);
             }
             if(strcmp(rcvg,"completed")==0)
             {
                 printf("File is transferred...\n");
                 fclose(fp);
                 break;
             }
             else
             {
                 fputs(rcvg,stdout);
                 fprintf(fp,"%s",rcvg);
                 
             }
    }
   
    // Close the socket:
    close(socket_desc);
    
    return 0;
}