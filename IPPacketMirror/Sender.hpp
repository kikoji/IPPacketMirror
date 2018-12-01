//
//  Sender.hpp
//  IPPacketMirror
//
//  Created by 岸田 好司 on 2018/11/29.
//  Copyright © 2018 岸田 好司. All rights reserved.
//

#ifndef Sender_hpp
#define Sender_hpp

#include <stdio.h>
#include "Config.hpp"

class Sender {
    int sockfd ;
    struct sockaddr_in addr ;
    
public:
    Sender( Config::AddrInfo& ) ;
    ~Sender() ;
    void forward( const Config& cf, char *p, unsigned int length ) ;
    
    // for print
    u_short sport ;
    u_short dport ;
    //struct  in_addr saddr ;
    //struct  in_addr daddr ;
    struct sockaddr_in saddr ;
    struct sockaddr_in daddr ;
    void printQueueAddr( struct  in_addr src, struct  in_addr dst) ;
    void printQueuePort( u_short src, u_short dst ) ;
};

#endif /* Sender_hpp */
