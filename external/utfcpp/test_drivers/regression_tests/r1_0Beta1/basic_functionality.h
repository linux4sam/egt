#include "../../../source/utf8.h"
using namespace utf8;

// [ 1528544 ] utf::next does not work correctly for 4-byte sequences
void id_1528544()
{
    unsigned char u10ffff[] = {0xf4, 0x8f, 0xbf, 0xbf};
    unsigned char* uit = u10ffff;
    try {
        unsigned int cp_u10ffff = next (uit, u10ffff + 4);
        check (cp_u10ffff == 0x10ffff);  
    }
    catch (std::exception&) {
        check (false);
    }
}
