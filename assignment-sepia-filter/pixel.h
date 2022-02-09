#ifndef __PIXEL_H__
#define __PIXEL_H__

struct pixel  {
    unsigned char b, g, r;
};

// Parse color with form RRGGBB
// If failed to parse, return black pixel instead.
struct pixel parse_RRGGBB(char* const str);

// maybe color math here, idk

#endif // __PIXEL_H__

