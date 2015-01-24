#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>


#include "bsd_radix/radix.h"
#include "route/route.hpp"

#define LEN(x) ( (int) (*(const u_char *)(x)) )
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

struct my_node {
    struct radix_node rn_node[2];
    struct sockaddr_in* sain;
};

int
main(int argc, char** argv)
{
    class route route("./route/fullroute_20140114.txt");

    struct radix_node_head* rnh = NULL;
    rn_inithead((void**)&rnh, 0);

    for (int i=0; i<route.size(); i++) {
        struct my_node* mn = (struct my_node*)malloc(sizeof(struct my_node));
        memset(mn, 0, sizeof(struct my_node));
        //route.memdump(&route.sin_value[i], sizeof(struct sockaddr_in));
        mn->sain = &route.sin_value[i];
        rn_addroute((void*)&route.sin_key[i], 
                (void*)&route.sin_mask[i], rnh, (struct radix_node*)mn);
    }

    //printf("check_start\n");
    
    TCHK_START(lookup);
    for (int i=0; i<route.size(); i++) {
        struct my_node* mn;
        /*
        mn = (struct my_node*)rn_lookup((void*)&route.sin_key[i], 
                                        (void*)&route.sin_mask[i], rnh);
        */
        mn = (struct my_node*)rn_match((void*)&route.sin_key[i], rnh);

        /*
        if (mn == NULL) {
            //printf("NULL\n");
        } else {
            //route.memdump(mn->sain, sizeof(struct sockaddr_in));
        }
        */
    }
    TCHK_END(lookup);

    return 0;
}
