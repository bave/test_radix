#include <iostream>

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "route/route.hpp"
#include "radixter/radix_tree.hpp"

#define TCHK_START(name)           \
    struct timeval name##_prev;    \
    struct timeval name##_current; \
    gettimeofday(&name##_prev, NULL)

#define TCHK_END(name)                                                             \
gettimeofday(&name##_current, NULL);                                               \
time_t name##_sec;                                                                 \
suseconds_t name##_usec;                                                           \
if (name##_current.tv_sec == name##_prev.tv_sec) {                                 \
    name##_sec = name##_current.tv_sec - name##_prev.tv_sec;                       \
    name##_usec = name##_current.tv_usec - name##_prev.tv_usec;                    \
} else if (name ##_current.tv_sec != name##_prev.tv_sec) {                         \
    int name##_carry = 1000000;                                                    \
    name##_sec = name##_current.tv_sec - name##_prev.tv_sec;                       \
    if (name##_prev.tv_usec > name##_current.tv_usec) {                            \
        name##_usec = name##_carry - name##_prev.tv_usec + name##_current.tv_usec; \
        name##_sec--;                                                              \
        if (name##_usec > name##_carry) {                                          \
            name##_usec = name##_usec - name##_carry;                              \
            name##_sec++;                                                          \
        }                                                                          \
    } else {                                                                       \
        name##_usec = name##_current.tv_usec - name##_prev.tv_usec;                \
    }                                                                              \
}                                                                                  \
printf("%s: sec:%lu usec:%06ld\n", #name, name##_sec, name##_usec); 

class rtentry {
public:
        uint32_t addr;
        int prefix_len;

        rtentry() : addr(0), prefix_len(0) { }

        uint32_t operator[] (int n) const {
            uint32_t bit;

            if (addr & (0x80000000 >> n)) {
                bit = 1;
            } else {
                bit = 0;
            }

            return bit;
        }

        bool operator== (const rtentry &rhs) const {
            return prefix_len == rhs.prefix_len && addr == rhs.addr;
        }

        bool operator< (const rtentry &rhs) const {
            if (addr == rhs.addr) {
                return prefix_len < rhs.prefix_len;
            } else {
                return addr < rhs.addr;
            }
        }
};

int
radix_length(const rtentry &entry)
{
    return entry.prefix_len;
}

rtentry
radix_substr(const rtentry &entry, int begin, int num)
{
    rtentry ret;
    uint32_t mask;

    if (num == 32) {
        mask = 0;
    } else {
        mask = 1 << num;
    }

    mask -= 1;
    mask <<= 32 - num - begin;

    ret.addr = (entry.addr & mask) << begin;
    ret.prefix_len = num;

    return ret;
}

int
main(int argc, char **argv)
{
    class route route("./route/fullroute_20140114.txt");
    class radix_tree<rtentry, uint32_t> rttable;
    class rtentry entry;

    for (int i=0; i<route.size(); i++) {
        entry.addr = ntohl(route.n_key[i]);
        entry.prefix_len = atoi(route.mask[i].c_str());

        /*
        printf("%d\n", i);
        route.memdump(&entry.addr, sizeof(uint32_t));
        route.memdump(&entry.prefix_len, sizeof(int));
        */

        rttable[entry] = ntohl(route.n_value[i]);
    }

    
    //printf("check_start\n");

    TCHK_START(hoge);
    for (int i=0; i<route.size(); i++) {
        entry.addr = ntohl(route.n_key[i]);
        entry.prefix_len = 32;
        rttable.longest_match(entry);
    }
    TCHK_END(hoge);

    return 0;
}
