#ifndef OVERLAY
#define OVERLAY

#include "utils.h"

extern void _init_overlay_display(void);
extern void _clear_overlay_display(void);
extern void _draw_overlay_pixel(u8 col, u8 x, u8 y);
extern void __draw_overlay_v_line(u8 col, u8 x, u8 y, u8 height);
extern void __draw_overlay_h_line(u8 col, u8 x, u8 y, u8 width);
extern void __draw_overlay_dotted_v_line(u8 col, u8 x, u8 y, u8 height);
extern void __draw_overlay_dotted_h_line(u8 col, u8 x, u8 y, u8 width);

#endif//OVERLAY