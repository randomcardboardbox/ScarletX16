#ifndef BRUSH_TOOL
#define BRUSH_TOOL

#include "../utils.h"
extern void draw_brush_to_sprite(u8 x, u8 y, u8 colour, u8 brush_size, u8 brush_type, u8 redraw_screen);
extern void draw_brush_left_hemisphere(u8 x, u8 y, u8 colour, u8 brush_size, u8 brush_type);
extern void draw_brush_right_hemisphere(u8 x, u8 y, u8 colour, u8 brush_size, u8 brush_type);
extern void draw_brush_lower_hemisphere(u8 x, u8 y, u8 colour, u8 brush_size, u8 brush_type);
extern void draw_brush_upper_hemisphere(u8 x, u8 y, u8 colour, u8 brush_size, u8 brush_type);

extern void _clear_render_queue(u8 end_y, u8 start_y);
extern void _draw_row_to_render_queue(u8 start_x, u8 end_x, u8 y);
extern void _draw_render_queue_to_sprite(u8 col, u8 end_y, u8 start_y);

#endif//BRUSH_TOOL