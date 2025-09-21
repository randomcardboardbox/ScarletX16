#include "utils.h"
#include "ui_utils.h"
#include "sprite_tools/tool_globals.h"
#include "paint_canvas.h"
#include "overlay_render_utils.h"

u8 overlay_sprite_offset_x = 96;
u8 overlay_sprite_offset_y = 40;

void draw_overlay_filled_rect(u8 col, u8 x, u8 y, u8 width, u8 height){
    u8 i;
    for(i=0; i<height; i++){
        _draw_overlay_h_line(col, x, y+i, width);
    }
}

void draw_overlay_rect(u8 col, u8 x, u8 y, u8 width, u8 height){
    _draw_overlay_h_line(col, x, y, width);
    _draw_overlay_h_line(col, x, y+height, width+1);
    _draw_overlay_v_line(col, x, y, height);
    _draw_overlay_v_line(col, x+width, y, height+1);
}

void draw_overlay_rect_from_points(u8 col, u8 x0, u8 y0, u8 x1, u8 y1){
    if(x0 < x1){
        _draw_overlay_h_line(col, x0, y0, x1-x0);
        _draw_overlay_h_line(col, x0, y1, x1-x0);
    }
    else{
        _draw_overlay_h_line(col, x1, y0, x0-x1);
        _draw_overlay_h_line(col, x1, y1, x0-x1);
    }
    if(y0 < y1){
        _draw_overlay_v_line(col, x0, y0, y1-y0);
        _draw_overlay_v_line(col, x1, y0, y1-y0);
    }
    else{
        _draw_overlay_v_line(col, x0, y1, y0-y1);
        _draw_overlay_v_line(col, x1, y1, y0-y1);
    }
}

void draw_overlay_filled_circle(){

}

u8 overlay_off_x;
u8 overlay_off_y;

u8 old_x;
u8 old_y;
u8 old_size;
void brush_tool_overlay(u8 just_clear){
    u8 brush_radius = brush_size >> 1;
    u8 x = (_mouse_data[MOUSE_X]>>(4-_canvas_pow_scale))-brush_radius;
    u8 y = (_mouse_data[MOUSE_Y]>>(4-_canvas_pow_scale))-brush_radius;
    u8 size = brush_size<<(4-_canvas_pow_scale);
    
    x = x << (4-_canvas_pow_scale);
    y = y << (4-_canvas_pow_scale);

    if(just_clear){
        draw_overlay_rect(0, old_x, old_y, old_size, old_size);
    }
    else{
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
}

u8 shape_old_x;
u8 shape_old_y;
u8 old_was_sel;
shape_tool_overlay(u8 just_clear){
    u8 brush_radius = brush_size >> 1;
    u8 x = (_mouse_data[MOUSE_X]>>(4-_canvas_pow_scale));
    u8 y = (_mouse_data[MOUSE_Y]>>(4-_canvas_pow_scale));
    x = x << (4-_canvas_pow_scale);
    y = y << (4-_canvas_pow_scale);

    if(x != shape_old_x || y != shape_old_y || point_selected != old_was_sel){
        draw_overlay_rect_from_points(0, 
            overlay_sprite_offset_x+(previous_point_x<<(4-_canvas_pow_scale)), 
            overlay_sprite_offset_y+(previous_point_y<<(4-_canvas_pow_scale)), 
            shape_old_x, 
            shape_old_y);

        if(point_selected){
            draw_overlay_rect_from_points(2, 
                overlay_sprite_offset_x+(previous_point_x<<(4-_canvas_pow_scale)), 
                overlay_sprite_offset_y+(previous_point_y<<(4-_canvas_pow_scale)), 
                x, 
                y);
        }

        shape_old_x = x;
        shape_old_y = y;
        old_was_sel = point_selected;
    }
}

u8 overlay_old_tool;
void overlay_routines(){
    if(overlay_old_tool != _current_tool){
        if(overlay_old_tool == DRAW_TOOL) brush_tool_overlay(1);
        else if(overlay_old_tool == RECT_TOOL) shape_tool_overlay(1);

        overlay_old_tool = _current_tool;
    }

    if(_current_tool == DRAW_TOOL) brush_tool_overlay(0);
    else if(_current_tool == RECT_TOOL) shape_tool_overlay(0);
}