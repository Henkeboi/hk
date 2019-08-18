#ifndef __FILE__HPP__
#define __FILE__HPP__

#include "buffer.hpp"

class file {
public:
    file() = delete;
    file(std::string filename);
    bool open(std::string filename);
    int get_left_margin();
    bool save(std::string filename);
    void insert_symbols(int x_curser, int line_number, std::wstring symbols);
    void insert_symbol(int x_curser, int line_number, wint_t symbol);
    void insert_newline(int x_curser, int line_number);
    void delete_symbol(int x_curser, int line_number);
    void delete_symbols(int x_start, int y_start, int x_end, int y_end);
    std::wstring get_line(int line_number);
    bool is_end_line(int line_number);
    int get_number_of_lines();  
    int next_word_index(int x_index, int line_number, int word_count);
    int prev_word_index(int x_index, int line_number, int word_count);
    void resize();
private:
    int _tab_size;
    int _left_margin;
    int _max_line_size;
    buffer _buffer;
};

#endif // __FILE__HPP__
