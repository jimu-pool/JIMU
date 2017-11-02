//
//  client.c
//  
//
//  Created by Sourgroup on 4/7/16.
//
//

#include "server.h"
#ifdef ALICE
void opensocket(int portno)
{
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    /*  if (argc < 2) {
     fprintf(stderr,"ERROR, no port provided\n");
     exit(1);
     }
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
   // portno=12344;
    //   portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    int on=1;
    if((setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    close(sockfd);
    stream = fdopen(newsockfd, "wb+");
    printf("connected\n");
    return;
}
#endif
