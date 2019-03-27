#include "../../../source/utf8.h"
using namespace utf8;

// [ 1531740 ] utf8::append does not work correctly for some code points.
void id_1531740()
{
    unsigned cp_u3044 = 0x3044U;
    unsigned char u3044[] = {0x0, 0x0, 0x0, 0x0};
    append(cp_u3044, u3044);
    check (u3044[0] == 0xe3 && u3044[1] == 0x81 && u3044[2] == 0x84 && u3044[3] == 0);
}
