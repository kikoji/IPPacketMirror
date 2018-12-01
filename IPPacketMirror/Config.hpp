//
//  Config.hpp
//  IPPacketMirror
//
//  Created by 岸田 好司 on 2018/11/28.
//  Copyright © 2018 岸田 好司. All rights reserved.
//

#ifndef Config_hpp
#define Config_hpp

#include <stdio.h>
#include <string>
#include <netinet/in.h>

class Sender ;

class Config {
public:
    struct AddrInfo {
        struct  in_addr addr ;
        u_short port ;
    } ;
    // モニタ対象（フィルタ条件）
    AddrInfo source ;
    AddrInfo destination ;
    std::string dev ;
    
    // 転送先
    AddrInfo foward ;
    Sender* sender ;
    
    /*
    enum eProtocol {
        eTcp = 1,
        eUdp = 2
    } protocol ;
    */
    std::string protocol ;
    
    enum ePrint {
        ePrintNone = 0 ,
        ePrint = 1,
        ePrintFull =2
    } print ;
    
    Config( int argc, const char* argv[] ) ;
    ~Config() ;
    
};

#endif /* Config_hpp */
