#include "../../../source/utf8.h"
using namespace utf8;

// [ 1538338 ] unchecked::next does not work correctly for 4-byte sequences.
void id_1538338()
{
    const char* four_bytes = "\xf0\x90\x8d\x86";
    const char* it = four_bytes;
    int cp = unchecked::next(it);
    check (cp == 0x10346);
}
