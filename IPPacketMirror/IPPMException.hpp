//
//  IPPMException.hpp
//  IPPacketMirror
//
//  Created by 岸田 好司 on 2018/11/29.
//  Copyright © 2018 岸田 好司. All rights reserved.
//

#ifndef IPPMException_hpp
#define IPPMException_hpp

#include <stdio.h>
#include <string>

class IPPMException {
public:
    std::string detail ;
    
    IPPMException( std::string str ){
        detail = str ;
    }
};

#endif /* IPPMException_hpp */
