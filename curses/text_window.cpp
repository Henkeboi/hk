#include "text_window.hpp"
#include "cassert"
#include "curses.h"

#define SELECT_COLOR 1

text_window::text_window(std::string filename)
: _file(filename), _left_margin(_file.get_left_margin()), _right_margin(1), _max_line_size(COLS - _left_margin - _right_margin),_x_curser(_left_margin), _y_curser(0), _buffer_start(0), _scrolling_buffer_start(0), _select_start(std::make_tuple(-1, -1)), _select_in_progress(false)
{
    resize();
}

bool text_window::open_file(std::string filename)
{
    return _file.open(filename);
}

bool text_window::save_file(std::string filename)
{
    return _file.save(filename);
}

void text_window::display()
{
    clear();
    int end_line = _buffer_start + LINES - 2;
    if (end_line >= _file.get_number_of_lines()) {
        end_line = _file.get_number_of_lines() - 1;
    }

    auto display_line_number = [this](int current_line) {
        std::string current_line_symbols = std::to_string(current_line);
        attroff(COLOR_PAIR(SELECT_COLOR));
        use_default_colors();
        for (int x_printer = 0; x_printer < _left_margin; ++x_printer) {
            if (x_printer < current_line_symbols.size()) {
                addch(current_line_symbols.at(x_printer));
            } else {
                addch(char(32)); // whitespace
            }
        }
    };

    auto display_relative_line_number = [this](int current_line) {
        if (current_line < _y_curser + _buffer_start) {
            current_line = _y_curser + _buffer_start - current_line; 
        } else if (current_line > _y_curser + _buffer_start) {
            current_line -= _y_curser + _buffer_start;
        }
        std::string current_line_symbols = std::to_string(current_line);
        attroff(COLOR_PAIR(SELECT_COLOR));
        use_default_colors();
        for (int x_printer = 0; x_printer < _left_margin; ++x_printer) {
            if (x_printer < current_line_symbols.size()) {
                addch(current_line_symbols.at(x_printer));
            } else {
                addch(char(32)); // whitespace
            }
        }

    };


    auto display_text = [this](int current_line) {
        std::wstring current_line_symbols{_file.get_line(current_line)};
        for (int x_printer = 0; x_printer < current_line_symbols.size(); ++x_printer) {
            if (_use_select_color(x_printer, current_line)) {
                attron(COLOR_PAIR(SELECT_COLOR));
            } else {
                attroff(COLOR_PAIR(SELECT_COLOR));
                use_default_colors();
            }
            std::wstring s;
            wint_t symbol = current_line_symbols.at(x_printer);
            s += symbol;
            wchar_t wide_char[] = { s.at(0), 0x00 };
            addwstr(wide_char);
        }
    };

    
    
    int y_curser = 0;
    if (_buffer_start < 0) {
        y_curser -= _buffer_start;
    }
    if (_y_curser < LINES - 1) {
        if (_buffer_start < 0) {
            for (int line_number = 0; line_number <= end_line; ++line_number) {
                move(y_curser, 0);
                //display_line_number(line_number);
                display_relative_line_number(line_number);
                display_text(line_number);
                ++y_curser;
            }
        } else {
            for (int line_number = _buffer_start; line_number <= end_line; ++line_number) {
                move(y_curser, 0);
                //display_line_number(line_number);
                display_relative_line_number(line_number);
                display_text(line_number);
                ++y_curser;
            }
        }
    }
    move(_y_curser, _x_curser);
    _scrolling_buffer_start = _buffer_start;
}

void text_window::display(int buffer_start)
{
    if (_scrolling_buffer_start < -((LINES - 2)/ 2)) {
        _scrolling_buffer_start += (LINES - 2) / 4;
    }
    int old_scrolling_buffer_start = _scrolling_buffer_start;
    int old_buffer_start = _buffer_start;
    _buffer_start = _scrolling_buffer_start;
    display();
    _scrolling_buffer_start = old_scrolling_buffer_start;
    _buffer_start = old_buffer_start;
}
    
void text_window::insert_symbols(std::wstring symbols)
{
    if (_y_curser + _buffer_start >= _file.get_number_of_lines()) return;
    _file.insert_symbols(_x_curser, _y_curser + _buffer_start, symbols);
    int old_y = _y_curser;
    curser_right(symbols.size());
    if (old_y != _y_curser) curser_right(1);
}

void text_window::insert(wint_t symbol)
{
    if (_y_curser + _buffer_start >= _file.get_number_of_lines()) return;
    _file.insert_symbol(_x_curser, _y_curser + _buffer_start, symbol);
    int old_y = _y_curser;
    curser_right(1); 
    if (old_y != _y_curser) curser_right(1);
}

void text_window::insert_newline()
{
    if (_y_curser + _buffer_start >= _file.get_number_of_lines()) return;
    _file.insert_newline(_x_curser, _y_curser + _buffer_start);
    curser_right(1);
}

void text_window::delete_symbol()
{
    if (_x_curser == _left_margin) {
        int old_x = _x_curser;
        if (_file.get_number_of_lines() > 1 && _y_curser + _buffer_start != 0) {
            curser_left(1); 
            _file.delete_symbol(old_x, _y_curser + _buffer_start + 1); 
        }
    } else {
        _file.delete_symbol(_x_curser, _y_curser + _buffer_start); 
        curser_left(1);
    }
}

void text_window::auto_indent()
{
    if (_y_curser == 0 && _buffer_start == 0 || _y_curser + _buffer_start > _file.get_number_of_lines()) return;

    auto space_count = [this](int line_number) {
        std::wstring line = _file.get_line(line_number);
        if (line.size() == 0) return 0;
        int counter = 0;
        while (counter < line.size()) {
            if (line.at(counter) == ' ' || line.at(counter) == '\n') {
                ++counter;
            } else {
                return counter;
            }
        }
        return counter;
    }(_y_curser + _buffer_start - 1);
    if (space_count != 0) {
        std::wstring indent = std::wstring(space_count, ' ');
        insert_symbols(indent); 
    }
}

void text_window::curser_down(int jump)
{
    curs_set(1);
    _y_curser += jump;
    int buffer_jump = 0;
    if (_y_curser > LINES - 2) {
        buffer_jump = _y_curser - LINES + 2;
        _y_curser = LINES - 2;
    }
    buffer_down(buffer_jump);
    if (_y_curser + _buffer_start >= 0) {
        if (_y_curser + _buffer_start >= _file.get_number_of_lines()) {
            _y_curser = _file.get_number_of_lines() - _buffer_start - 1;
        }
        if (_x_curser >= _file.get_line(_y_curser + _buffer_start).size() + _left_margin) {
            _x_curser = _file.get_line(_y_curser + _buffer_start).size() + _left_margin;
        }
    }
}

void text_window::curser_up(int jump)
{
    curs_set(1);
    if (_y_curser + _buffer_start < jump) {
        jump = _y_curser + _buffer_start;
    }
    _y_curser -= jump;
    int buffer_jump = 0;
    if (_y_curser < 0) {
        buffer_jump = -_y_curser;
        _y_curser = 0;
    }
    buffer_up(buffer_jump);
    if (_y_curser + _buffer_start >= 0) {
        if (_y_curser + _buffer_start < _file.get_number_of_lines()) {
            if (_x_curser >= _file.get_line(_y_curser + _buffer_start).size() + _left_margin) {
                _x_curser = _file.get_line(_y_curser + _buffer_start).size() + _left_margin;
            }
        }
    }
}

void text_window::curser_left(int jump)
{
    curs_set(1);
    if (_x_curser == _left_margin && _y_curser == 0 && _buffer_start == 0) {
        return;
    } else if (_y_curser + _buffer_start >= _file.get_number_of_lines()) {
        curser_up(1);
    } else {
        _x_curser -= jump;
        while (_x_curser < _left_margin) {                 
            curser_up(1);
            if (_file.get_line(_buffer_start + _y_curser).size()== 0) {
                _x_curser += 1;
            }
            else {
                _x_curser += _file.get_line(_buffer_start + _y_curser).size() + 1;
            }
        }
    }
}

void text_window::curser_right(int jump)
{
   curs_set(1);
    if (_y_curser + _buffer_start == _file.get_number_of_lines() - 1 && _x_curser == _file.get_line(_y_curser + _buffer_start).size() + _left_margin) {
        return;
    } else if (_y_curser + _buffer_start >= _file.get_number_of_lines()) {
        curser_down(1);
    } else {
        _x_curser += jump;
        while (_x_curser >= _file.get_line(_y_curser + _buffer_start).size() + _left_margin + 1) {
            _x_curser -= _file.get_line(_y_curser + _buffer_start).size() + 1;
            curser_down(1);
        }
    }
}

void text_window::buffer_down(int jump)
{
    _buffer_start += jump;
    if (_buffer_start >= _file.get_number_of_lines()) {
        _buffer_start = _file.get_number_of_lines();
    }
}

void text_window::buffer_up(int jump)
{
    _buffer_start -= jump;
}

void text_window::curser_next_word(int word_count)
{
    if (_y_curser + _buffer_start >= _file.get_number_of_lines()) return;
    int line_index = _x_curser - _left_margin;
    int distance = _file.next_word_index(line_index, _y_curser + _buffer_start, word_count) - line_index;
    if (_file.get_line(_y_curser + _buffer_start).size() - _x_curser + _left_margin <= distance) {
        _x_curser = _file.get_line(_y_curser + _buffer_start).size() + _left_margin;
        if (_file.get_line(_y_curser + _buffer_start).size() != 0) _x_curser -= 1;
    } else {
        curser_right(distance);
    }
}

void text_window::curser_prev_word(int word_count)
{
    if (_y_curser + _buffer_start >= _file.get_number_of_lines()) return;
    int line_index = _x_curser - _left_margin;
    int distance =  line_index - _file.prev_word_index(line_index, _y_curser + _buffer_start, word_count);
    if (_x_curser - _left_margin < distance) {
        _x_curser = _left_margin;
    } else {
        curser_left(distance);
    }
}

void text_window::curser_start_line()
{
    _x_curser = _left_margin;
}

void text_window::curser_end_line()
{
    _x_curser = _file.get_line(_y_curser + _buffer_start).size() + _left_margin; 
}

void text_window::toggle_select()
{
    if (!_select_in_progress) {
        std::get<0>(_select_start) = _x_curser - _left_margin;
        std::get<1>(_select_start) = _y_curser + _buffer_start;
        _select_in_progress = true;
    } else {
        _select_in_progress = false;
        display();
    }
}

void text_window::delete_selection()
{
    if (!_select_in_progress) return; 
    int x_start = std::get<0>(_select_start);
    int y_start = std::get<1>(_select_start);
    int x_end = _x_curser - _left_margin;
    int y_end = _y_curser + _buffer_start;
    if (y_start == y_end) {
        if (x_start > x_end) {
            int temp = x_start;
            x_start = x_end;
            x_end = temp;
        }
    } else if (y_start > y_end) {
        int temp_x = x_start;
        int temp_y = y_start;
        x_start = x_end;
        x_end = temp_x; 
        y_start = y_end;
        y_end = temp_y;
    }
    _file.delete_symbols(x_start, y_start, x_end, y_end);
    toggle_select();
}

bool text_window::_use_select_color(int x_printer, int current_line)
{
    if (_select_in_progress) {
        auto y_in_selection_range = [current_line](int select_start, int y_curser) {
        if (y_curser >= select_start) {
                if (current_line <= y_curser && current_line >= select_start) {
                    return true;
                }
            } else {
                if (current_line >= y_curser && current_line <= select_start) {
                    return true;
                }
            }
            return false;
        }(std::get<1>(_select_start), _y_curser + _buffer_start);
        
        auto x_in_selection_range = [this, x_printer](int select_start, int x_curser) {
            if (select_start <= x_curser) {
                if (x_printer <= x_curser && x_printer >= select_start) return true;
            }
            if (select_start >= x_curser) {
                if (x_printer >= x_curser && x_printer <= select_start) return true;
            }
            return false;
        }(std::get<0>(_select_start) - _left_margin, _x_curser - _left_margin);

        if (current_line == std::get<1>(_select_start)) {
            if (_y_curser + _buffer_start > current_line) {
                if (x_printer >= std::get<0>(_select_start) - _left_margin) return true;
            } else if (_y_curser + _buffer_start < current_line) {
                if (x_printer <= std::get<0>(_select_start) - _left_margin) return true;
            } else if (x_in_selection_range) return true;
        } else if (_y_curser + _buffer_start == current_line) {
            if (_y_curser + _buffer_start > std::get<1>(_select_start)) {
                if (x_printer < _x_curser - _left_margin) return true;
            } else if (_y_curser + _buffer_start < std::get<1>(_select_start)) {
                if (x_printer > _x_curser - _left_margin) return true;
            }
        } else if (y_in_selection_range) return true;
    }
    return false;
}

void text_window::display_center()
{
    int old_y = _y_curser;
    int jump_distance = _y_curser - ((LINES - 2) / 2);
    if (jump_distance < 0)  {
        _y_curser = old_y - jump_distance;
        buffer_up(-jump_distance);
    } else {
        buffer_down(jump_distance);
        _y_curser = old_y - jump_distance;
    }
    display();
    curs_set(1);
}

void text_window::display_up(int jump)
{
    if (_scrolling_buffer_start == _buffer_start && _y_curser <= 3 * (LINES - 2) / 4) {
        _buffer_start -= (LINES - 2) / 4;
        _y_curser += (LINES - 2) / 4;
        display();
        curs_set(1);
    } else {
        _scrolling_buffer_start -= (LINES - 2) / 4;
        display(_scrolling_buffer_start);
        curs_set(0);
    }
    if (_scrolling_buffer_start == _buffer_start) curs_set(1);
}

void text_window::display_down(int jump)
{
    if (_scrolling_buffer_start == _buffer_start && _y_curser >= (LINES - 2) / 4) {
        _buffer_start += (LINES - 2) / 4;
        _y_curser -= (LINES - 2) / 4;
        display();
        curs_set(1);
    } else {
        _scrolling_buffer_start += (LINES - 2) / 4;
        display(_scrolling_buffer_start);
        curs_set(0);
    } 
    if (_scrolling_buffer_start == _buffer_start) curs_set(1);
}

void text_window::resize()
{
    auto new_line_number = _file.resize(_y_curser + _buffer_start);
    _buffer_start -= new_line_number - _y_curser - _buffer_start;
    display();
}
