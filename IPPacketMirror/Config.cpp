//
//  Config.cpp
//  IPPacketMirror
//
//  Created by 岸田 好司 on 2018/11/28.
//  Copyright © 2018 岸田 好司. All rights reserved.
//

#include "Config.hpp"
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include "IPPMException.hpp"
#include "Sender.hpp"

int setAddr( std::string str, Config::AddrInfo& info ){
    unsigned long pos = str.find(':');
    std::string adr = str.substr(0, pos);
    std::string port = str.substr(pos + 1);
    
    if( adr.length() != 0 ){
        info.addr.s_addr = inet_addr( adr.c_str() );
    }
    else{
        info.addr.s_addr = 0 ;
    } ;
    
    if( port.length() != 0 ){
        int p = atoi( port.c_str() ) ;
        info.port = htons( p ) ;
    }
    else{
        info.port = 0 ;
    } ;
    
    if( info.addr.s_addr == 0 || info.port == 0 ) return 0 ;
    else return 1 ;
}

Config::Config( int argc, const char* argv[] ){
    if( argc <= 1 ){
        IPPMException except("not enough parameter\n\nusage\n ippm <foward-addr:port> dev=<dev> p=<protocol> src=<addr:port> dst=<adr:port>\n\n ex-1.>ippm 192.168.1.1:9000 dev=en0 p=tcp src=192.168.1.1:80 dst=192.168.1.1:8000\n ex-2.>ippm 192.168.1.1:9000 dst=:8000  (defalut protoco is udp. default dev is en0. destination port=8000 filter)\n") ;
        throw except ;
    };
    
    std::string fwd(argv[1]) ;
    if( !setAddr( fwd, foward ) ) {
        IPPMException except("no forward address parameter") ;
        throw except ;
    };
    
    protocol = "udp" ;
    destination.addr.s_addr = 0 ;
    source.addr.s_addr = 0 ;
    dev = "en0" ;
    print = false ;
    for( int k = 2 ; k < argc ; k++ ){
        std::string param( argv[k] ) ;
        unsigned long pos = param.find('=');
        std::string attr = param.substr(0, pos);
        std::string info = param.substr(pos + 1);
        
        if( attr == "dst" ){
            setAddr( info, destination )  ;
        }
        else if( attr == "src" ){
            setAddr( info, source )  ;
        }
        else if( attr == "p" && ( info=="udp" || info  == "tcp") ){
            protocol = info ;
        }
        else if( attr == "dev" ){
            dev = info ;
        }
        else if( attr == "print" ){
            print = true ;
        }
        else{
            IPPMException except("illegal parameter ="+attr) ;
            throw except ;
        }
    };
    
    sender = new Sender( foward ) ;
}
