//
//  client.h
//  
//
//  Created by Sourgroup on 4/7/16.
//
//

#ifndef client_h
#define client_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "util.h"
void error(const char *msg);
void opensocket(int portno);
#endif /* client_h */
