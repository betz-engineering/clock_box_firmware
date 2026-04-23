#include <stdint.h>
#include <stdio.h>
#include <font.h>
// -----------------------------------
//  MissingPlanet
// -----------------------------------

static const uint8_t glyphs_missingplanet[473] = {
254,128,182,128, 82,190,165, 43,234, 80, 39,233, 74,124,165, 47,200,237,122, 34, 34,245,184,249,
 18, 34,130, 42,162, 74,226,224,242, 73, 36,224,228,146, 73,224,171,190,234,128, 33, 62, 66,  0,
 39,128,248,128,  8, 66, 34, 34, 16,128,252, 99, 24,198, 49,248,225,  8, 66, 16,132,248,248, 66,
 31,194, 16,248,248, 66, 31,132, 33,248,140, 99, 31,132, 33,  8,252, 33, 15,132, 33,248,252, 33,
 15,198, 49,248,248, 66, 16,132, 33,  8,252, 99, 31,198, 49,248,252, 99, 31,132, 33,248,136, 32,
  2,120, 58, 32,131,128,248, 62,224,130, 46,  0,252, 98, 19,144,128, 32,255,192,111,176, 91,237,
 22,255,  0,255,128,252, 99, 31,198, 49,136,242, 40, 98,242, 40, 98,240,252, 33,  8, 66, 16,248,
242, 40,161,134, 24,162,240,252, 33, 15,194, 16,248,252, 33, 15,194, 16,128,252, 99, 11,198, 49,
248,140, 99, 31,198, 49,136,255,128,  8, 66, 16,132, 33,248,134, 24,164,242, 72,161,132,132, 33,
  8, 66, 16,248,255,196, 98, 49, 24,140, 70, 35, 17,136,128,248,196, 98, 49, 24,140, 70, 35, 17,
143,128,252, 99, 24,198, 49,248,252, 99, 31,194, 16,128,252, 99, 24,198,181,249,  0,252, 99, 31,
202, 82,144,252, 33, 15,132, 33,248,249,  8, 66, 16,132, 32,140, 99, 24,198, 49,248,140, 99, 24,
198, 42, 32,136,196, 98, 49, 24,140, 70, 35, 17,255,128,140, 98,162, 42, 49,136,140, 99, 31,144,
132, 32,248, 66, 31,194, 16,248,242, 73, 36,224,132, 32,130,  8, 33,  8,228,146, 73,224, 84,248,
144,248,127, 31,128,132, 33, 15,198, 49,248,252, 33, 15,128,  8, 66, 31,198, 49,248,252,127, 15,
128, 57,  8, 79,144,132, 32,252, 99, 31,132, 33,248,132, 33, 15,198, 49,136,190, 32,146, 73, 39,
128,132, 33,  8,203,210,136,255,128,253,107, 90,128,252, 99, 24,128,252, 99, 31,128,252, 99, 31,
194, 16,128,252, 99, 31,132, 33,  8,252, 33,  8,  0,252, 62, 31,128, 33,  8, 79,144,132, 32,140,
 99, 31,128,140, 98,162,  0,173,107, 95,128,140, 93, 24,128,140, 99, 31,132, 33,248,248,127, 15,
128, 57,  8, 78, 16,132, 56,255,248,225,  8, 67,144,132,224,237,110
};

// GLYPH DESCRIPTION
static const glyph_description_t glyph_dsc_missingplanet[95] = {
    {.width =  0, .height =  0, .lsb =  0, .tsb =  0, .advance =  5, .start_index =  0, },  // U+0020 ' '
    {.width =  1, .height =  9, .lsb =  0, .tsb =  9, .advance =  2, .start_index =  0, },  // U+0021 '!'
    {.width =  3, .height =  3, .lsb =  0, .tsb =  9, .advance =  4, .start_index =  2, },  // U+0022 '"'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index =  4, },  // U+0023 '#'
    {.width =  5, .height = 11, .lsb =  0, .tsb = 10, .advance =  6, .start_index = 10, },  // U+0024 '$'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 17, },  // U+0025 '%'
    {.width =  7, .height =  9, .lsb =  0, .tsb =  9, .advance =  8, .start_index = 23, },  // U+0026 '&'
    {.width =  1, .height =  3, .lsb =  0, .tsb =  9, .advance =  2, .start_index = 31, },  // U+0027 '''
    {.width =  3, .height =  9, .lsb =  0, .tsb =  9, .advance =  4, .start_index = 32, },  // U+0028 '('
    {.width =  3, .height =  9, .lsb =  0, .tsb =  9, .advance =  4, .start_index = 36, },  // U+0029 ')'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 40, },  // U+002A '*'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  7, .advance =  6, .start_index = 44, },  // U+002B '+'
    {.width =  3, .height =  3, .lsb = -2, .tsb =  1, .advance =  2, .start_index = 48, },  // U+002C ','
    {.width =  5, .height =  1, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 50, },  // U+002D '-'
    {.width =  1, .height =  1, .lsb =  0, .tsb =  1, .advance =  2, .start_index = 51, },  // U+002E '.'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 52, },  // U+002F '/'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 58, },  // U+0030 '0'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 64, },  // U+0031 '1'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 70, },  // U+0032 '2'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 76, },  // U+0033 '3'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 82, },  // U+0034 '4'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 88, },  // U+0035 '5'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 94, },  // U+0036 '6'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 100, },  // U+0037 '7'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 106, },  // U+0038 '8'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 112, },  // U+0039 '9'
    {.width =  1, .height =  5, .lsb =  0, .tsb =  5, .advance =  2, .start_index = 118, },  // U+003A ':'
    {.width =  3, .height =  7, .lsb = -2, .tsb =  5, .advance =  2, .start_index = 119, },  // U+003B ';'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  7, .advance =  6, .start_index = 122, },  // U+003C '<'
    {.width =  5, .height =  3, .lsb =  0, .tsb =  6, .advance =  6, .start_index = 126, },  // U+003D '='
    {.width =  5, .height =  5, .lsb =  0, .tsb =  7, .advance =  6, .start_index = 128, },  // U+003E '>'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 132, },  // U+003F '?'
    {.width =  9, .height =  9, .lsb =  0, .tsb =  9, .advance = 10, .start_index = 138, },  // U+0040 '@'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 149, },  // U+0041 'A'
    {.width =  6, .height =  9, .lsb =  0, .tsb =  9, .advance =  7, .start_index = 155, },  // U+0042 'B'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 162, },  // U+0043 'C'
    {.width =  6, .height =  9, .lsb =  0, .tsb =  9, .advance =  7, .start_index = 168, },  // U+0044 'D'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 175, },  // U+0045 'E'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 181, },  // U+0046 'F'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 187, },  // U+0047 'G'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 193, },  // U+0048 'H'
    {.width =  1, .height =  9, .lsb =  0, .tsb =  9, .advance =  2, .start_index = 199, },  // U+0049 'I'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 201, },  // U+004A 'J'
    {.width =  6, .height =  9, .lsb =  0, .tsb =  9, .advance =  7, .start_index = 207, },  // U+004B 'K'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 214, },  // U+004C 'L'
    {.width =  9, .height =  9, .lsb =  0, .tsb =  9, .advance = 10, .start_index = 220, },  // U+004D 'M'
    {.width =  9, .height =  9, .lsb =  0, .tsb =  9, .advance = 10, .start_index = 231, },  // U+004E 'N'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 242, },  // U+004F 'O'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 248, },  // U+0050 'P'
    {.width =  5, .height = 10, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 254, },  // U+0051 'Q'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 261, },  // U+0052 'R'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 267, },  // U+0053 'S'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 273, },  // U+0054 'T'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 279, },  // U+0055 'U'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 285, },  // U+0056 'V'
    {.width =  9, .height =  9, .lsb =  0, .tsb =  9, .advance = 10, .start_index = 291, },  // U+0057 'W'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 302, },  // U+0058 'X'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 308, },  // U+0059 'Y'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 314, },  // U+005A 'Z'
    {.width =  3, .height =  9, .lsb =  0, .tsb =  9, .advance =  4, .start_index = 320, },  // U+005B '['
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 324, },  // U+005C '\'
    {.width =  3, .height =  9, .lsb =  0, .tsb =  9, .advance =  4, .start_index = 330, },  // U+005D ']'
    {.width =  3, .height =  2, .lsb =  0, .tsb =  9, .advance =  4, .start_index = 334, },  // U+005E '^'
    {.width =  5, .height =  1, .lsb =  0, .tsb = -1, .advance =  6, .start_index = 335, },  // U+005F '_'
    {.width =  2, .height =  2, .lsb =  0, .tsb =  9, .advance =  3, .start_index = 336, },  // U+0060 '`'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 337, },  // U+0061 'a'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 341, },  // U+0062 'b'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 347, },  // U+0063 'c'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 351, },  // U+0064 'd'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 357, },  // U+0065 'e'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 361, },  // U+0066 'f'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 367, },  // U+0067 'g'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 373, },  // U+0068 'h'
    {.width =  1, .height =  7, .lsb =  0, .tsb =  7, .advance =  2, .start_index = 379, },  // U+0069 'i'
    {.width =  3, .height = 11, .lsb = -2, .tsb =  7, .advance =  2, .start_index = 380, },  // U+006A 'j'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 385, },  // U+006B 'k'
    {.width =  1, .height =  9, .lsb =  0, .tsb =  9, .advance =  2, .start_index = 391, },  // U+006C 'l'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 393, },  // U+006D 'm'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 397, },  // U+006E 'n'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 401, },  // U+006F 'o'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 405, },  // U+0070 'p'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 411, },  // U+0071 'q'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 417, },  // U+0072 'r'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 421, },  // U+0073 's'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 425, },  // U+0074 't'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 431, },  // U+0075 'u'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 435, },  // U+0076 'v'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 439, },  // U+0077 'w'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 443, },  // U+0078 'x'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 447, },  // U+0079 'y'
    {.width =  5, .height =  5, .lsb =  0, .tsb =  5, .advance =  6, .start_index = 453, },  // U+007A 'z'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 457, },  // U+007B '{'
    {.width =  1, .height = 13, .lsb =  0, .tsb =  9, .advance =  2, .start_index = 463, },  // U+007C '|'
    {.width =  5, .height =  9, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 465, },  // U+007D '}'
    {.width =  5, .height =  3, .lsb =  0, .tsb =  9, .advance =  6, .start_index = 471, },  // U+007E '~'
};

const font_header_t f_missingplanet = {
    .magic = 0x005A54BE,
    .n_glyphs = 95,
    .map_start = 32,
    .map_n = 95,
    .map_table = NULL,
    .glyph_description_table = glyph_dsc_missingplanet,
    .glyph_data_table = glyphs_missingplanet,
    .linespace = 17,
    .flags = 0,
    .name = "MissingPlanet"
};
