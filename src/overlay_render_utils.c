#include "utils.h"
#include "ui_utils.h"
#include "sprite_tools/tool_globals.h"
#include "paint_canvas.h"
#include "overlay_render_utils.h"

void draw_overlay_filled_rect(u8 col, u8 x, u8 y, u8 width, u8 height){
    u8 i;
    for(i=0; i<height; i++){
        _draw_overlay_h_line(col, x, y+i, width);
    }
}

void draw_overlay_rect(u8 col, u8 x, u8 y, u8 width, u8 height){
    // _draw_overlay_pixel(col, x, y);
    // _draw_overlay_pixel(col, x+width, y+height);
    // _draw_overlay_pixel(col, x, y+height);
    // _draw_overlay_pixel(col, x+width, y);

    _draw_overlay_h_line(col, x, y, width);
    _draw_overlay_h_line(col, x, y+height, width);
    // _draw_overlay_v_line(col, x, y, height);
    // _draw_overlay_v_line(col, x+width, y, height);
}

void draw_overlay_filled_circle(){

}

u8 overlay_off_x;
u8 overlay_off_y;

u8 old_x;
u8 old_y;
u8 old_size;
void brush_tool_overlay(){
    u8 brush_radius = brush_size >> 1;
    u8 x = (_mouse_data[MOUSE_X]>>_canvas_pow_scale)-brush_radius;
    u8 y = (_mouse_data[MOUSE_Y]>>_canvas_pow_scale)-brush_radius;
    u8 size = brush_size<<_canvas_pow_scale;
    
    x = x << _canvas_pow_scale;
    y = y << _canvas_pow_scale;

    if(x != old_x || y != old_y || size != old_size){
        draw_overlay_rect(0, old_x, old_y, old_size, old_size);

        if(!was_drawing_last_frame){
            draw_overlay_rect(2, x, y, size, size);
        }

        old_x = x;
        old_y = y;
        old_size = size;
    }
}

void overlay_routines(){
    if(_current_tool == DRAW_TOOL) brush_tool_overlay();
}