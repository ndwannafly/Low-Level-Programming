#include "pixel.h"
#include <stdio.h>

struct pixel parse_RRGGBB(char* const str){
    struct pixel res = {0, 0, 0};

    for (int i = 0; i < 6; ++i){
        if (str[i] >= '0' && str[i] <= '9') continue;
	if (str[i] >= 'A' && str[i] <= 'F') continue;
	if (str[i] >= 'a' && str[i] <= 'f') continue;
	return res;
    }

    if(str[6]) return res;

    unsigned char* res_data = (unsigned char*) &res;
    sscanf(str, "%x", res_data);
    return res;
}
