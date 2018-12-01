//
//  Sender.cpp
//  IPPacketMirror
//
//  Created by 岸田 好司 on 2018/11/29.
//  Copyright © 2018 岸田 好司. All rights reserved.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // close
#include <iostream>
#include "Sender.hpp"
#include "IPPMException.hpp"

Sender::Sender( Config::AddrInfo& info ){
    addr.sin_family = AF_INET;
    addr.sin_port = info.port ;
    addr.sin_addr.s_addr = info.addr.s_addr ;
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if( sockfd <= 0 ){
        IPPMException except("cannot create socket") ;
        throw except ;
    }
}

Sender::~Sender(){
    close(sockfd);
}

#include <netinet/ip.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
void Sender::forward( const Config& cf, char *p, unsigned int length ){
    std::cout << inet_ntoa(addr.sin_addr) << std::endl ;
    for( int k = 0 ; k < length ; k++ ){
        printf("%02x",*p) ;
    };
    sendto(sockfd, p, length, 0, (struct sockaddr *)&addr, sizeof(addr));
}
