#include <stdio.h>

#include "brush_tool.h"

#include "../paint_canvas.h"
#include "../history_stack.h"
#include "../utils.h"
#include "../ui_utils.h"
#include "tool_ui.h"

u8 row_widths[] = {1, 1, 2, 2, 3, 3, 2, 3, 4, 4, 2, 3, 4, 5, 5, 3, 4, 5, 6, 6, 6, 3, 4, 5, 6, 7, 7, 7, 3, 5, 
    6, 7, 7, 8, 8, 8, 3, 5, 6, 7, 8, 8, 9, 9, 9, 3, 5, 6, 7, 8, 9, 9, 10, 10, 10, 4, 6, 7, 8, 9, 10, 10, 11, 
    11, 11, 11, 4, 6, 7, 8, 9, 10, 11, 11, 12, 12, 12, 12, 4, 6, 8, 9, 10, 11, 11, 12, 12, 13, 13, 13, 13, 4, 
    7, 8, 9, 10, 11, 12, 13, 13, 13, 14, 14, 14, 14, 4, 7, 9, 10, 11, 12, 13, 13, 14, 14, 14, 15, 15, 15, 15, 
    4, 7, 9, 10, 11, 12, 13, 14, 14, 15, 15, 15, 16, 16, 16, 16};
u8 brush_ptrs[17] = {0, 0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120};

#define REDNER_QUEUE_SIZE 256
#define RENDER_QUEUE_START_ADDR 0xA000
#define RENDER_QUEUE_END_ADDR 0xA000+REDNER_QUEUE_SIZE
#define RENDER_QUEUE_BANK 3

void draw_brush_to_render_queue(u8 x, u8 y, u8 colour, u8 brush_size, u8 brush_type){
    if(brush_type == 0){ //round brush
        u8 radius = (brush_size+1)>>1;
        u8 width_index = brush_ptrs[radius];
        i16 width;
        i16 i;
        u8 odd_brush_size = brush_size&(0b00000001);

        if(odd_brush_size){
            i16 start_y1 = (i16)y-(i16)radius;
            i16 start_y2 = (i16)y+(i16)radius-2;
            i16 start_x;
            u8 row_width;

            for(i=0; i<radius; i++){
                width = row_widths[width_index];
                width_index += 1;
                start_x = x-width;
                row_width = (width<<1)-1;
                
                if(start_x < 0){
                    start_x = 0;
                    row_width -= width-x;
                }
                if(start_x+row_width >= (i16)(*bmx_height)){
                    row_width -= (start_x+row_width) - (i16)(*bmx_height);
                    if(row_width <= 0) continue;
                }

                if(start_y1+i > 0 && start_y1+i < (*bmx_height)) _draw_row_to_render_queue(start_x, start_x+row_width, start_y1+i);
                if(start_y1+i > 0 && start_y1+i < (*bmx_height)) _draw_row_to_render_queue(start_x, start_x+row_width, start_y2-i);
            }
        }
        else{
            i16 start_y1 = (i16)y-(i16)radius;
            i16 start_y2 = (i16)y+(i16)radius-1;
            i16 start_x;
            i16 row_width;

            for(i=0; i<radius; i++){
                width = row_widths[width_index];
                width_index += 1;
                start_x = x-width;
                row_width = (width<<1);

                if(start_x < 0){
                    start_x = 0;
                    row_width -= width-x;
                }
                if(start_x+row_width >= (i16)(*bmx_height)){
                    row_width -= (start_x+row_width) - (i16)(*bmx_height);
                    if(row_width <= 0) continue;
                }

                if(start_y1+i >= 0 && start_y1+i < (*bmx_height)) _draw_row_to_render_queue(start_x, start_x+row_width, start_y1+i);
                if(start_y2-i >= 0 && start_y2-i < (*bmx_height)) _draw_row_to_render_queue(start_x, start_x+row_width, start_y2-i);
            }
        }
    }
    else{ //square brush
        i16 i;
        u8 radius = (brush_size+1)>>1;
        i16 start_x = x-radius;
        i16 start_y = (i16)y-(i16)radius;
        u8 width = brush_size;

        if(x < radius){
            start_x = 0;
            width -= radius - x;
        }
        else if(start_x+brush_size > (*bmx_width)){
            width -= (start_x+brush_size) - (*bmx_width);
        }

        for(i=0; i<brush_size; i++){
            if(start_y+i >= (i16)(*bmx_height)) break;
            if(start_y+i >= (i16)0) _draw_row_to_render_queue(start_x, start_x+width, start_y+i);
        }
    }
}

void draw_brush_to_sprite(u8 x, u8 y, u8 colour, u8 brush_size, u8 brush_type, u8 redraw_screen){
    if(brush_type == 0){ //round brush
        u8 radius = (brush_size+1)>>1;
        u8 width_index = brush_ptrs[radius];
        i16 width;
        i16 i;
        u8 odd_brush_size = brush_size&(0b00000001);

        if(odd_brush_size){
            i16 start_y1 = (i16)y-(i16)radius;
            i16 start_y2 = (i16)y+(i16)radius-2;
            i16 start_x;
            u8 row_width;

            for(i=0; i<radius; i++){
                width = row_widths[width_index];
                width_index += 1;
                start_x = x-width;
                row_width = (width<<1)-1;
                
                if(start_x < 0){
                    start_x = 0;
                    row_width -= width-x;
                }
                if(start_x+row_width >= (i16)(*bmx_height)){
                    row_width -= (start_x+row_width) - (i16)(*bmx_height);
                    if(row_width <= 0) continue;
                }
                add_history_node_row((width<<1)-1, x-width, y+i-radius, colour);
                add_history_node_row((width<<1)-1, x-width, y+radius-i-2, colour);

                if(start_y1+i > 0 && start_y1+i < (*bmx_height)) _draw_row_to_sprite(colour, row_width, start_x, start_y1+i);
                if(start_y1+i > 0 && start_y1+i < (*bmx_height)) _draw_row_to_sprite(colour, row_width, start_x, start_y2-i);
            }
        }
        else{
            i16 start_y1 = (i16)y-(i16)radius;
            i16 start_y2 = (i16)y+(i16)radius-1;
            i16 start_x;
            i16 row_width;

            for(i=0; i<radius; i++){
                width = row_widths[width_index];
                width_index += 1;
                start_x = x-width;
                row_width = (width<<1);

                if(start_x < 0){
                    start_x = 0;
                    row_width -= width-x;
                }
                if(start_x+row_width >= (i16)(*bmx_height)){
                    row_width -= (start_x+row_width) - (i16)(*bmx_height);
                    if(row_width <= 0) continue;
                }
                _add_history_node_row((width<<1), x-width, y+i-radius, colour);
                _add_history_node_row((width<<1), x-width, y+radius-i-1, colour);

                if(start_y1+i >= 0 && start_y1+i < (*bmx_height)) {
                    _draw_row_to_sprite(colour, row_width, start_x, start_y1+i);
                    if(redraw_screen) _draw_row_to_screen(start_y1+i);
                }
                if(start_y2-i >= 0 && start_y2-i < (*bmx_height)) {
                    _draw_row_to_sprite(colour, row_width, start_x, start_y2-i);
                    if(redraw_screen) _draw_row_to_screen(start_y2-i);
                }
            }
        }
    }
    else{ //square brush
        i16 i;
        u8 radius = (brush_size+1)>>1;
        i16 start_x = x-radius;
        i16 start_y = (i16)y-(i16)radius;
        u8 width = brush_size;

        if(x < radius){
            start_x = 0;
            width -= radius - x;
        }
        else if(start_x+brush_size > (*bmx_width)){
            width -= (start_x+brush_size) - (*bmx_width);
        }

        for(i=0; i<brush_size; i++){
            _add_history_node_row(width, start_x, start_y+i, colour);
            if(start_y+i >= (i16)(*bmx_height)) break;
            if(start_y+i >= (i16)0){
                _draw_row_to_sprite(colour, width, start_x, start_y+i);
                if(redraw_screen) _draw_row_to_screen(start_y+i);
            }
        }
    }
}

void draw_brush_right_hemisphere(u8 x, u8 y, u8 colour, u8 brush_size, u8 brush_type){
    u8 radius = brush_size>>1;
    u8 width_index = brush_ptrs[radius];
    u8 width;
    u8 i;
    u8 j;
    for(i=0; i<radius; i++){
        width = row_widths[width_index];
        width_index += 1;
        // add_history_node_row((width<<1), x-width, y+radius-i-1, colour);
        _draw_column_to_sprite(colour, (width<<1), x+radius-i-1, y-width);
    }
}

void draw_brush_left_hemisphere(u8 x, u8 y, u8 colour, u8 brush_size, u8 brush_type){
    u8 radius = brush_size>>1;
    u8 width_index = brush_ptrs[radius];
    u8 width;
    u8 i;
    u8 j;
    for(i=0; i<radius; i++){
        width = row_widths[width_index];
        width_index += 1;

        // add_history_node_row((width<<1), x-width, y+i-radius, colour);
        _draw_column_to_sprite(colour, (width<<1), x+i-radius, y-width);
    }
}

void draw_brush_lower_hemisphere(u8 x, u8 y, u8 colour, u8 brush_size, u8 brush_type){
    u8 radius = brush_size>>1;
    u8 width_index = brush_ptrs[radius];
    u8 width;
    u8 i;
    u8 j;
    for(i=0; i<radius; i++){
        width = row_widths[width_index];
        width_index += 1;
        // add_history_node_row((width<<1), x-width, y+radius-i-1, colour);
        _draw_row_to_sprite(colour, (width<<1), x-width, y+radius-i-1);
    }
}

void draw_brush_upper_hemisphere(u8 x, u8 y, u8 colour, u8 brush_size, u8 brush_type){
    u8 radius = brush_size>>1;
    u8 width_index = brush_ptrs[radius];
    u8 width;
    u8 i;
    u8 j;
    for(i=0; i<radius; i++){
        width = row_widths[width_index];
        width_index += 1;

        // add_history_node_row((width<<1), x-width, y+i-radius, colour);
        _draw_row_to_sprite(colour, (width<<1), x-width, y+i-radius);
    }
}