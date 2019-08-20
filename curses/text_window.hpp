#ifndef __TEXT__WINDOW__HPP__
#define __TEXT__WINDOW__HPP__

#include "file.hpp"
#include <memory>
#include <string>

class text_window {
public:
    text_window() = delete;
    text_window(std::string filename);
    bool open_file(std::string filename);
    bool save_file(std::string filename);
    void display();
    void insert_symbols(std::wstring symbols);
    void insert(wint_t symbol);
    void insert_newline();
    void delete_symbol();
    void auto_indent();
    void curser_down(int jump); 
    void curser_left(int jump);
    void curser_right(int jump);
    void curser_up(int jump);
    void curser_next_word(int word_count);
    void curser_prev_word(int word_count);
    void curser_start_line();
    void curser_end_line();
    void toggle_select();
    void delete_selection();
    void copy_selection();
    void display_center();
    void display_up(int jump);
    void display_down(int jump);
    void resize();
private:
    void display(int buffer_start);
    void buffer_up(int jump);
    void buffer_down(int jump);
    bool _use_select_color(int current_x, int current_y);
    file _file; 
    int _left_margin; 
    int _right_margin; 
    int _max_line_size;
    int _x_curser;
    int _y_curser;
    int _buffer_start;
    int _scrolling_buffer_start;
    std::tuple<int, int> _select_start;
    bool _select_in_progress;
};

#endif // __TEXT__WINDOW__HPP__
