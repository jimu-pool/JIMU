//
//  client.c
//  
//
//  Created by Sourgroup on 4/7/16.
//
//

#include "client.h"
#ifdef BOB
void opensocket(int portno)
{
    int sockfd,  n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname("localhost");//gethostbyname(argv[1]);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    while(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) == -1) {
        usleep(1000);
    }
    stream = fdopen(sockfd, "wb+");
    printf("connected\n");
    return;
}
#endif
