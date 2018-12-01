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
#include <net/ethernet.h>
#include <arpa/inet.h>
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

void Sender::forward( const Config& cf, char *p, unsigned int length ){
    //std::cout << inet_ntoa(addr.sin_addr) << std::endl ;
    
    sendto(sockfd, p, length, 0, (struct sockaddr *)&addr, sizeof(addr));
    
    if( cf.print == Config::ePrint || cf.print == Config::ePrintFull ){
        std::string srcAddr( inet_ntoa(saddr.sin_addr) ) ;
        std::string dstAddr( inet_ntoa(daddr.sin_addr) ) ;
        printf("%s %s:%d->%s:%d[%d] ",cf.protocol.c_str(),srcAddr.c_str(),ntohs(sport),dstAddr.c_str(),ntohs(dport), length) ;
    
        if( cf.print == Config::ePrintFull ){
            char *t = p ;
            for( int k = 0 ; k < length ; k++ ){
                printf("%02x ", (*t++) & 0x00FF ) ;
            };
        };
        printf("\n") ;
    };
}

void Sender::printQueueAddr( struct  in_addr src, struct  in_addr dst ){
    saddr.sin_addr.s_addr = src.s_addr ;
    daddr.sin_addr.s_addr = dst.s_addr ;
}

void Sender::printQueuePort( u_short s, u_short d){
    sport = s ;
    dport = d ;
}
