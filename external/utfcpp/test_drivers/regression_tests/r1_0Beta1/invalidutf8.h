#include "../../../source/utf8.h"
using namespace utf8;

/// [ 1524459 ] utf8::is_valid does not report some illegal code positions
void id_1524459()
{
// Single UTF-16 surrogates: 
unsigned char ud800[] = {0xed, 0xa0, 0x80};
check (!is_valid(ud800, ud800 + 3));

unsigned char udb7f[] = {0xed, 0xad, 0xbf};
check (!is_valid(udb7f, udb7f + 3));
  
unsigned char udb80[] = {0xed, 0xae, 0x80};
check (!is_valid(udb80, udb80 + 3));
  
unsigned char udbff[] = {0xed, 0xaf, 0xbf};
check (!is_valid(udbff, udbff + 3));

unsigned char udc00[] = {0xed, 0xb0, 0x80};
check (!is_valid(udc00, udc00 + 3));

unsigned char udf80[] = {0xed, 0xbe, 0x80};
check (!is_valid(udf80, udf80 + 3));

unsigned char udfff[] = {0xed, 0xbf, 0xbf};
check (!is_valid(udfff, udfff + 3));

// Paired UTF-16 surrogates: 
unsigned char ud800_dc00[] = {0xed, 0xa0, 0x80, 0xed, 0xb0, 0x80};
check (!is_valid(ud800_dc00, ud800_dc00 + 6));

unsigned char ud800_dfff[] = {0xed, 0xa0, 0x80, 0xed, 0xbf, 0xbf};
check (!is_valid(ud800_dfff, ud800_dfff + 6));

unsigned char udb7f_dc00[] = {0xed, 0xad, 0xbf, 0xed, 0xb0, 0x80};
check (!is_valid(udb7f_dc00, udb7f_dc00 + 6));

unsigned char udb7f_dfff[] = {0xed, 0xad, 0xbf, 0xed, 0xbf, 0xbf};
check (!is_valid(udb7f_dfff, udb7f_dfff + 6));

unsigned char udb80_dc00[] = {0xed, 0xae, 0x80, 0xed, 0xb0, 0x80};
check (!is_valid(udb80_dc00, udb80_dc00 + 6));

unsigned char udb80_dfff[] = {0xed, 0xae, 0x80, 0xed, 0xbf, 0xbf};
check (!is_valid(udb80_dfff, udb80_dfff + 6));

unsigned char udbff_dc00[] = {0xed, 0xaf, 0xbf, 0xed, 0xb0, 0x80};
check (!is_valid(udbff_dc00, udbff_dc00 + 6));

unsigned char udbff_dfff[] = {0xed, 0xaf, 0xbf, 0xed, 0xbf, 0xbf};
check (!is_valid(udbff_dfff, udbff_dfff + 6));
}

// [ 1525236 ] utf8::is_valid does not detect overlong sequences
void id_1525236 ()
{
unsigned char u2f_2bytes[] = {0xc0, 0xaf};
check (!is_valid(u2f_2bytes, u2f_2bytes + 2));

unsigned char u2f_3bytes[] = {0xe0, 0x80, 0xaf};
check (!is_valid(u2f_3bytes, u2f_3bytes + 3));

unsigned char u2f_4bytes[] = {0xf0, 0x80, 0x80, 0xaf};
check (!is_valid(u2f_4bytes, u2f_4bytes + 4));
}

// [ 1528369 ] utf8::find_invalid does not return the start of a seqence
void id_1528369 ()
{
// incomplete utf sequences  
unsigned char utf_incomplete[] = {0xe6, 0x97, 0x0};
unsigned char* invalid = find_invalid(utf_incomplete, utf_incomplete + 3);
check (invalid == utf_incomplete);

unsigned char utf_incomplete_two_seqs[] = {0xE6, 0x97, 0xA5, 0xd1, 0x0};
invalid = find_invalid(utf_incomplete_two_seqs, utf_incomplete_two_seqs + 5);
check (invalid == utf_incomplete_two_seqs + 3);

// invalid code point
unsigned char udbff[] = {0xed, 0xaf, 0xbf};
invalid = find_invalid(udbff, udbff + 3);
check (invalid == udbff);

// overlong sequence
unsigned char u2f_3bytes[] = {0xe0, 0x80, 0xaf};
invalid = find_invalid(u2f_3bytes, u2f_3bytes + 3);
check (invalid == u2f_3bytes);
}
