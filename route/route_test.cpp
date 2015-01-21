#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include "rr.hpp"
#include "radix.h"

#define LEN(x) ( (int) (*(const u_char *)(x)) )

int
main(int argc, char** argv)
{
    class rr route("./fullroute_20140114.txt");
    std::cout << route.keys(2) << std::endl;
    std::cout << route.masks(2) << std::endl;
    std::cout << route.values(2) << std::endl;
    std::cout << route.key[2] << std::endl;
    std::cout << route.mask[2] << std::endl;
    std::cout << route.value[2] << std::endl;

    printf("%x\n", route.n_key[2]);
    printf("%x\n", route.n_mask[2]);
    printf("%x\n", route.n_value[2]);
    return EXIT_SUCCESS;
}



/*
struct my_node {
    struct radix_node rn_node[2];
    int my_value;
};

int
main(int argc, char** argv)
{
    struct radix_node_head* rnh = NULL;
    //memset(&rnh, 0, sizeof(struct radix_node_head));
    //rn_init(1);
    rn_inithead((void**)&rnh, 0);

    char key =   0x01;
    char mask1  = 0x00;
    char mask2  = 0x01;

    struct my_node mn;
    memset(&mn, 0, sizeof(struct my_node));
    mn.my_value = 10;

    rn_addroute((void*)&key, (void*)&mask1, rnh, (struct radix_node*)&mn);

    struct my_node* mn_ptr = (struct my_node*)rn_lookup((void*)&key, (void*)&mask2, rnh);
    printf("%d\n", mn_ptr->my_value);

    return 0;
}
*/
