//
//  main.cpp
//  IPPacketMirror
//
//  Created by 岸田 好司 on 2018/11/28.
//  Copyright © 2018 岸田 好司. All rights reserved.
//

#include <iostream>
#include <pcap.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
//#include <sys/socket.h>
//#include <net/if_dl.h>

#include "Config.hpp"
#include "IPPMException.hpp"
#include "Sender.hpp"

void analyzeUDPPacket( const Config& cf, char *p, unsigned int udpLength){
    struct udphdr *udp ;
    udp = (struct udphdr*) p ;
    
    if( cf.source.port != 0 && cf.source.port != udp->uh_sport ) return ;
    if( cf.destination.port != 0 && cf.destination.port != udp->uh_dport ) return ;
    cf.sender->printQueuePort( udp->uh_sport, udp->uh_dport ) ;
    
    p += 8 ;
    unsigned int payloadLength = udpLength - 8 ;
    cf.sender->forward( cf, p, payloadLength ) ;
}

void analyzeTCPPacket( const Config& cf, char *p, unsigned int tcpLength){
    struct tcphdr *tcp ;
    tcp = (struct tcphdr*) p ;
    
    if( cf.source.port != 0 && cf.source.port != tcp->th_sport ) return ;
    if( cf.destination.port != 0 && cf.destination.port != tcp->th_dport ) return ;
    cf.sender->printQueuePort( tcp->th_sport, tcp->th_dport ) ;
    
    p += tcp->th_off * 4 ;
    unsigned int payloadLength = tcpLength - tcp->th_off * 4 ;
    cf.sender->forward( cf, p, payloadLength ) ;
}

void analyzeIPPacket( const Config& cf, char *p)
{
    struct ip *ip;
    ip = (struct ip *)p;
    
    if( cf.source.addr.s_addr != 0 && cf.source.addr.s_addr != ip->ip_src.s_addr ) return ;
    if( cf.destination.addr.s_addr != 0 && cf.destination.addr.s_addr != ip->ip_dst.s_addr ) return ;
    cf.sender->printQueueAddr( ip->ip_src, ip->ip_dst ) ;
    
    p +=  ip->ip_hl * 4 ;
    unsigned int payloadLength = ntohs(ip->ip_len) - ip->ip_hl * 4 ;
    switch ( ip->ip_p ) {
        case IPPROTO_TCP :
            analyzeTCPPacket( cf, p, payloadLength)  ;
            break;
            
        case IPPROTO_UDP :
            analyzeUDPPacket( cf, p, payloadLength)  ;
            break ;
            
        default:
            break;
    };
}


//
int main(int argc, const char * argv[]) {
    // refeence code
    // https://netwiz.jp/tcpip/write_packetcapture.html
    // Thank you.
    
    try {
        Config config( argc, argv ) ;
        
        pcap_t *handle;
        char errbuf[PCAP_ERRBUF_SIZE];
        /* 受信用のデバイスを開く */
        if ((handle = pcap_open_live(config.dev.c_str(), BUFSIZ, 1, 1000, errbuf)) == NULL) {
        //if ((handle = pcap_open_live("en0", BUFSIZ, 1, 1000, errbuf)) == NULL) {
            //fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
            IPPMException ex("Couldn't open device:"+config.dev+" err="+std::string(errbuf)) ;
            throw ex ;
        }

        /* イーサネットのみ */
        /*
        if (pcap_datalink(handle) != DLT_EN10MB) {
            //fprintf(stderr, "Device not support: %s\n", dev);
            IPPMException ex("Device not support:"+config.dev) ;
            throw ex ;
        }
        */
        
        /* プロトコルの設定をおこなう */
        struct bpf_program fp;
        bpf_u_int32 net;
        if (pcap_compile(handle, &fp, config.protocol.c_str(), 0, net) == -1) {
            //fprintf(stderr, "Couldn't parse filter: %s\n", pcap_geterr(handle));
            IPPMException ex("Couldn't parse filter:"+std::string(pcap_geterr(handle)) ) ;
            throw ex ;
        }
        if (pcap_setfilter(handle, &fp) == -1) {
            //fprintf(stderr, "Couldn't install filter: %s\n", pcap_geterr(handle));
            IPPMException ex("Couldn't install filter:"+std::string(pcap_geterr(handle)) ) ;
            throw ex ;
        }
        
        /* ループでパケットを受信 */
        struct pcap_pkthdr header;
        const unsigned char *packet;
        int l1headerSize = sizeof(struct ether_header) ;
        if( config.dev.substr(0,2) == "lo" ) l1headerSize = 4 ;
        while (1) {
            if ((packet = pcap_next(handle, &header)) == NULL) continue;
            /* イーサネットヘッダーとIPヘッダーの合計サイズに満たなければ無視 */
            if (header.len < l1headerSize+sizeof(struct ip)) continue;
            analyzeIPPacket( config, (char *)(packet+l1headerSize) ) ;
        }
        pcap_close(handle);
    }
    catch (IPPMException& ex){
        std::cout << ex.detail << std::endl ;
        exit(EXIT_FAILURE);
    };
    
    return 0;
}
