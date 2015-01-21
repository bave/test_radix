#include <iostream>

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "route/route.hpp"
#include "radixter/radix_tree.hpp"

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

rtentry
radix_join(const rtentry &entry1, const rtentry &entry2)
{
    rtentry ret;

    ret.addr        = entry1.addr;
    ret.addr       |= entry2.addr >> entry1.prefix_len;
    ret.prefix_len  = entry1.prefix_len + entry2.prefix_len;

    return ret;
}

int
main(int argc, char **argv)
{
    class route route("./route/fullroute_20140114.txt");
    class radix_tree<rtentry, uint32_t> rttable;
    class rtentry entry;

    for (int i=0; i<route.size(); i++) {
        entry.addr = route.n_key[i];
        entry.prefix_len = atoi(route.mask[i].c_str());

        printf("%d\n", i);
        route.memdump(&entry.addr, sizeof(uint32_t));
        route.memdump(&entry.prefix_len, sizeof(int));

        rttable[entry] = route.n_value[i];
    }

    /*
    add_rtentry("0.0.0.0", 0, "192.168.0.1");
    find_route("172.20.0.1");
    rm_rtentry("10.0.0.0", 8);
    find_route("10.1.1.1");
    */

    return 0;
}
