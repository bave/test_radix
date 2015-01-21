#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>

/*
 * format
 * 192.168.255.1/24 192.168.255.1
 * :
 * 192.168.255.254/24 192.168.255.1
 */

class route
{
public:
    route() { }

    route(const std::string& s) { init(s); }

    route(const char* s) { init(std::string(s)); }

    virtual ~route() {
        key.clear();
        mask.clear();
        value.clear();
    }

    bool init(const std::string& s)
    {
        std::ifstream ifs;
        ifs.open(s, std::ios::in);
        if(ifs.fail()) {
            return false;
        }

        std::string tmp;
        std::vector<std::string> vtmp;
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_len = sizeof(addr);
        addr.sin_family = AF_INET;
        addr.sin_port = INADDR_ANY;
        while(getline(ifs, tmp)) {
            vtmp.clear();
            split(tmp, " ", vtmp);
            tmp = vtmp[0];
            value.push_back(vtmp[1]);
            inet_pton(AF_INET, vtmp[1].c_str(), &addr.sin_addr);
            n_value.push_back(addr.sin_addr.s_addr);
            sin_value.push_back(addr);
            //memdump(&addr, sizeof(addr));


            vtmp.clear();
            split(tmp, "/", vtmp);
            key.push_back(vtmp[0]);
            if(inet_pton(AF_INET, vtmp[0].c_str(), &addr.sin_addr) == 1) { 
                n_key.push_back(addr.sin_addr.s_addr);
            } else {
                n_key.push_back(0);
                addr.sin_addr.s_addr = INADDR_ANY;
            }
            sin_key.push_back(addr);
            //memdump(&addr, sizeof(addr));

            mask.push_back(vtmp[1]);
            uint32_t mask_addr = 0xFFFFFFFF<<(32-atoi(vtmp[1].c_str()));
            n_mask.push_back(htonl(mask_addr));
            addr.sin_addr.s_addr = htonl(mask_addr);
            sin_mask.push_back(addr);
            //memdump(&addr, sizeof(addr));
            //printf("\n");
        }
        return true;
    }

    inline int size() {
        return key.size();
    }

    inline std::string& keys(int i) {
        return key[i];
    }

    inline std::string& masks(int i) {
        return mask[i];
    }

    inline std::string& values(int i) {
        return value[i];
    }

    std::vector<std::string> key;
    std::vector<std::string> mask;
    std::vector<std::string> value;

    std::vector<uint32_t> n_key;
    std::vector<uint32_t> n_mask;
    std::vector<uint32_t> n_value;

    std::vector<struct sockaddr_in> sin_key;
    std::vector<struct sockaddr_in> sin_mask;
    std::vector<struct sockaddr_in> sin_value;

    void memdump(void* buffer, int length)
    {

        uint32_t* addr32 = (uint32_t*)buffer;
        int i;
        int j;
        int k;
        int lines = length/16 + (length%16?1:0);
        if (lines > 1 || length == 16) {
            for (i=0; i<lines; i++) {
                printf("%p : %08x %08x %08x %08x\n",
                        addr32,
                        htonl(*(addr32)),
                        htonl(*(addr32+1)),
                        htonl(*(addr32+2)),
                        htonl(*(addr32+3))
                      );
                addr32 += 4;
            }
        } else {
        }

        j = length%16;
        if (j == 0) {
            return;
        }

        k = 0;
        uint8_t*  addr8 = (uint8_t*)addr32;
        printf("%p : ", addr8);
        for (i=0; i<16; i++) {
            if (k%4 == 0 && i != 0) printf(" ");
            if (j > i) {
                printf("%02x", *addr8);
                addr8++;
            } else {
                printf("XX");
            }
            k++;
        }
        printf("\n");

        return;
    }

private:
    void split(const std::string& src, const char *c, std::vector<std::string>& dst)
    {
        std::string::size_type i = 0;
        std::string::size_type j = src.find(c);
        std::string tmp = src.substr(i,j-i);

        if (tmp.size() == 0) return;
        dst.push_back(tmp);

        while(j != std::string::npos){
            i = j++;
            j = src.find(c,j);
            if (j == std::string::npos){
                dst.push_back(src.substr(i+1, src.size()));
                break;
            }
            tmp = src.substr(i,j-i);
            dst.push_back(tmp.substr(1,tmp.size()));
        }
        return;
    }
};
