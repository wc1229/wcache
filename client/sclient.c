#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

int main(int argc, char *argv[])
{ 
    if(argc != 3){
      printf("Usage: %s <IP address> <output file>\n", argv[0]); 
      return 1;
    } 

    int sockfd;
    int len; 
    struct sockaddr_in address; 
    int result; 
    char httpstring[100]; 
    char buffer[MAX_BUF_SIZE];
    char ch;
    FILE *output;

    sprintf(httpstring, "GET /%s HTTP/1.1\r\n"
          "Host: %s\r\n"
          "Connection: Close\r\n\r\n", argv[2], argv[1]); 

    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = inet_addr(argv[1]); 
    address.sin_port = htons(80); 
    len = sizeof(address);
    result = connect(sockfd, (struct sockaddr *)&address, len); 

    if(result == -1){ 
       perror("connect error"); 
       return 1; 
    }

    write(sockfd, httpstring, strlen(httpstring)); 

    output = fopen(argv[2], "w");
    if(output == NULL){
        perror("failed to open output file");
        close(sockfd);
        return 1;
    }

    // while(read(sockfd, buffer, MAX_BUF_SIZE) > 0){ 
    //   fwrite(buffer, sizeof(char), strlen(buffer), output);
    // } 

    int end_of_header = 0; // 标记响应头是否结束
    int n;

    while(read(sockfd, &ch, 1)){ 
        if(!end_of_header){ // 读取响应头
            printf("%c", ch); 
            if(ch == '\r'){ // 如果遇到 '\r'，说明一行已经结束了
                n = read(sockfd, &ch, 1);
                printf("%c", ch); 
                n = read(sockfd, &ch, 1);
                printf("%c", ch); 
                if(ch == '\r'){ // 如果下一行是空行，则说明响应头结束
                    end_of_header = 1;
                    n = read(sockfd, &ch, 1);
                    printf("%c", ch); 
                }
            }
        }else{ // 读取响应内容
            fwrite(&ch, sizeof(char), 1, output);
        }
    } 

    fclose(output);
    close(sockfd); 
    printf("The response has been saved to %s\n", argv[2]); 
    return 0; 
}