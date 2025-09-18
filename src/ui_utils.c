#include "ui_utils.h"
#include <stdio.h>

u8 create_ui_element(u8 parent_id, u8 type, u8 pos_x, u8 pos_y, u8 size_x, u8 size_y, u16 render_func, u16 mouse_func){
    u8 i;
    for(i=1; i<MAX_UI_ELEMENTS; i++){
        if(_ui_type[i] == 0){
            _ui_type[i] = type;
            _ui_position_x[i] = pos_x;
            _ui_position_y[i] = pos_y;
            _ui_size_x[i] = size_x;
            _ui_size_y[i] = size_y;

            _ui_rend_func_low[i] = render_func;
            _ui_rend_func_high[i] = render_func>>8;
            _ui_on_mouse[i] = mouse_func;
            
            _ui_prev_sib[i] = 0;
            _ui_next_sib[i] = 0;
            _ui_no_of_children[i] = 0;
            _ui_first_child[i] = 0;
            _ui_last_child[i] = 0;
            _ui_parent[i] = parent_id;

            _ui_var_ptr_low[i] = NULL;
            _ui_var_ptr_high[i] = NULL;

            if(_ui_last_child[parent_id] != 0){
                u8 last_child_id = _ui_last_child[parent_id];
                
                _ui_next_sib[last_child_id] = i;
                _ui_prev_sib[i] = last_child_id;
            }
            else{
                _ui_first_child[parent_id] = i;
            }

            _ui_last_child[parent_id] = i;
            _ui_no_of_children[parent_id] += 1;

            return(i);
        }
    }
}

u8 delete_ui_element(u8 ui_id){
    u8 curr_ui_id = _ui_first_child[ui_id];
    u8 parent_id = _ui_parent[ui_id];

    _ui_type[ui_id] = 0;
    _ui_no_of_children[parent_id] -= 1; 
    if(_ui_prev_sib[ui_id] != 0){
        u8 prev_sib = _ui_prev_sib[ui_id];
        _ui_next_sib[prev_sib] = _ui_next_sib[ui_id];
        if(_ui_next_sib[ui_id] == 0) _ui_last_child[parent_id] = prev_sib;
    }
    else {
        _ui_first_child[parent_id] = _ui_next_sib[ui_id];
        if(_ui_next_sib[ui_id] == 0) _ui_last_child[parent_id] = 0;
    }

    while(curr_ui_id != 0){
        _ui_type[curr_ui_id] = 0;
        if(_ui_first_child[curr_ui_id] != 0){
            curr_ui_id = _ui_first_child[curr_ui_id];
        }
        else if(_ui_next_sib[curr_ui_id] != 0){
            curr_ui_id = _ui_next_sib[curr_ui_id];
        }
        else{
            u8 curr_parent = _ui_parent[curr_ui_id];
            if(curr_parent == ui_id) break;
            curr_ui_id = _ui_next_sib[curr_parent];
        }
    }
}

//TODO: handling text changes and button changes
void update_ui_elements_from_ptr(){
    u8 i;
    for(i=0; i<MAX_UI_ELEMENTS; i++){
        if(_ui_type[i] != 0){
            u8 *ptr = (u16)_ui_var_ptr_low[i] + (((u16)_ui_var_ptr_high[i])<<8);
            if(ptr != NULL){
                if(_ui_type[i] == UI_TOGGLE_BUT){
                    if(*ptr != _ui_var_old_val[i]){
                        if(*ptr != _ui_var_val[i]) _ui_palette[i] = 0xF0; 
                        else _ui_palette[i] = 0xE0; 
                        _draw_ui_element(i);
                    }
                }
                else if(*ptr != _ui_var_old_val[i]){
                    
                    _draw_ui_element(i);
                }

                _ui_var_old_val[i] = *ptr;
            }
        }
    }
}

u8 keycode;
void get_keycode(){
    asm("jsr $FFE4");
    asm("sta %v", keycode);
}

void parse_mouse_input(){
    
    u8 mouse_x = (_mouse_data[MOUSE_X]>>3)+1;
    u8 mouse_y = (_mouse_data[MOUSE_Y]>>3)+1;
    u8 mouse_buttons = _mouse_data[MOUSE_BUTTONS];


    if(mouse_buttons & M_LEFT_BUT){
        u8 id = check_mouse_over_ui(0, mouse_x, mouse_y);
        if(id != 0 && _ui_on_mouse[id] != NULL){
            void (*fptr)(u8) = _ui_on_mouse[id];
            fptr(id);
        }
    }
}

u8 check_mouse_over_ui(u8 ui_ind, u8 mouse_x, u8 mouse_y){
    u8 pos_x = _ui_global_pos_x[ui_ind];
    u8 pos_y = _ui_global_pos_y[ui_ind];
    u8 size_x = _ui_size_x[ui_ind];
    u8 size_y = _ui_size_y[ui_ind];

    if((mouse_x > pos_x && mouse_x <= pos_x+size_x) &&
    (mouse_y > pos_y && mouse_y <= pos_y+size_y)){
        u8 new_id = _ui_first_child[ui_ind];    
        if(new_id != 0){
            while(1){
                u8 check_id = check_mouse_over_ui(new_id, mouse_x, mouse_y);
                if(check_id != 0){
                    return(check_id);
                }
                new_id = _ui_next_sib[new_id];
                if(new_id == 0){
                    return(ui_ind);
                }
            }
        }
        return(ui_ind);
    }
    else{
        return(0);
    }
}

void init_text_element(u8 ui_id, u16 text_ptr){
    _ui_var_ptr_low[ui_id] = text_ptr;
    _ui_var_ptr_high[ui_id] = text_ptr>>8;
}

void init_icon_element(u8 ui_id, u8 icon_addr, u16 variable_addr, u8 variable_value){
    u8* val_ptr = variable_addr;
    _ui_var_1[ui_id] = icon_addr;
    _ui_var_ptr_low[ui_id] = variable_addr;
    _ui_var_ptr_high[ui_id] = variable_addr>>8;
    _ui_var_val[ui_id] = variable_value;

    if(*val_ptr == variable_value){
        _ui_palette[ui_id] = 0xE0;
    }
}

void init_slider_element(u8 ui_id, u16 variable_addr){
    _ui_var_ptr_low[ui_id] = variable_addr;
    _ui_var_ptr_high[ui_id] = variable_addr>>8;
}