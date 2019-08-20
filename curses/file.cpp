#include "file.hpp"
#include <fstream>
#include <sstream>
#include "curses.h"
#include "cassert"
#include <codecvt>

file::file(std::string filename)
: _tab_size(4)
{
    open(filename);
    _left_margin = std::to_string(_buffer.get_number_of_lines()).size() + 1;
    _max_line_size = COLS - _left_margin - 1;
}

bool file::open(std::string filename)
{
    try {
        // read unicode from file
        std::wifstream wif(filename);
        wif.imbue(std::locale(std::locale(""), new std::codecvt_utf8<wchar_t>));
        std::wstringstream file;
        file << wif.rdbuf();

        std::wstring line;
        while (std::getline(file, line)) {
            _buffer.get_lines().push_back(std::make_tuple(std::wstring{}, true));
            int x_curser = 0;
            for (int i = 0; i < line.size(); ++i) {
                if (x_curser == _max_line_size) {
                    std::get<1>(_buffer.get_lines().back()) = false;
                    _buffer.get_lines().push_back(std::make_tuple(std::wstring{}, true));
                    x_curser = 0;
                }
                std::wstring s;
                s += line.at(i);
                wchar_t wide_char[] = { s.at(0), 0x00 };
                std::get<0>(_buffer.get_lines().back()) += wide_char; 
                ++x_curser;
            }
        }

        // init _buffer.get_lines() if file is empty
        if (_buffer.get_number_of_lines() == 0) {
            _buffer.get_lines().push_back(std::make_tuple(std::wstring{}, true));
        }
        return true;
    } catch (...) {
    }
    return false;
}

int file::get_left_margin()
{
    return _left_margin;
}

bool file::save(std::string filename)
{
    auto string_to_write = [this]() {
        std::wstring new_file_string;
        for (auto line_info : _buffer.get_lines()) {
            for (auto symbol : std::get<0>(line_info)) {
                new_file_string += symbol;
            }
            if (std::get<1>(line_info) == true) {
                wchar_t wide_char[] = { 10, 0x00 };
                new_file_string += wide_char;
            }
        }
        return new_file_string;
    }();
    std::wofstream wof(filename);
    wof.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
    wof << string_to_write;
    return true;
}

void file::insert_symbols(int x_curser, int line_number, std::wstring symbols)
{ 
    if (x_curser - _left_margin >= get_line(line_number).size()) std::get<0>(_buffer.get_lines().at(line_number)) += symbols;
    else std::get<0>(_buffer.get_lines().at(line_number)).insert(x_curser - _left_margin, symbols);
    if (get_line(line_number).size() > _max_line_size) {
        if (is_end_line(line_number) == true) {
            std::get<1>(_buffer.get_lines().at(line_number)) = false;
            auto next_line_iter = _buffer.get_lines().begin() + line_number + 1;
            _buffer.get_lines().insert(next_line_iter, std::make_tuple(std::wstring{}, true));
        }
        auto next_line = std::get<0>(_buffer.get_lines().at(line_number)).substr(_max_line_size);
        std::get<0>(_buffer.get_lines().at(line_number)) = std::get<0>(_buffer.get_lines().at(line_number)).substr(0, _max_line_size);
        insert_symbols(_left_margin, line_number + 1, next_line);
    }
}

void file::insert_symbol(int x_curser, int line_number, wint_t symbol)
{
    assert(line_number < _buffer.get_number_of_lines());
    auto simple_insert_symbol = [this](int x_curser, int line_number, wint_t symbol) {
        std::wstring s;
        wint_t c = symbol;
        s += c;
        wchar_t wide_char[] = {s.at(0), 0x00};
        std::get<0>(_buffer.get_lines().at(line_number)).insert(x_curser - _left_margin, wide_char);
    };

    if (is_end_line(line_number) == true) {
        simple_insert_symbol(x_curser, line_number, symbol);
        if (std::get<0>(_buffer.get_lines().at(line_number)).size() > _max_line_size) {
            auto next_line_symbol = std::get<0>(_buffer.get_lines().at(line_number)).back();
            auto next_line_tuple = std::make_tuple(std::wstring{next_line_symbol}, true);
            auto next_line_iter = _buffer.get_lines().begin() + line_number + 1;
            _buffer.get_lines().insert(next_line_iter, next_line_tuple);
            std::get<1>(_buffer.get_lines().at(line_number)) = false;
            std::get<0>(_buffer.get_lines().at(line_number)).pop_back();
        }
    } else {
        simple_insert_symbol(x_curser, line_number, symbol);
        auto next_line_symbol = std::get<0>(_buffer.get_lines().at(line_number)).back(); 
        std::get<0>(_buffer.get_lines().at(line_number)).pop_back();
        insert_symbol(_left_margin, line_number + 1, next_line_symbol);
    }
}

void file::insert_newline(int x_curser, int line_number)
{
    assert(line_number < _buffer.get_lines().size());
    auto current_line = std::get<0>(_buffer.get_lines().at(line_number));
    std::wstring next_line = L"";
    auto next_line_iter = _buffer.get_lines().begin() + line_number + 1;
    next_line += current_line.substr(x_curser - _left_margin, current_line.size());
    _buffer.get_lines().insert(next_line_iter, std::make_tuple(next_line, true));
    auto prev_line = current_line.substr(0, x_curser - _left_margin);
    _buffer.get_lines().at(line_number) = std::make_tuple(prev_line, true);
}

void file::delete_symbol(int x_curser, int line_number)
{
    assert(line_number < _buffer.get_lines().size());
    if (x_curser - _left_margin == 0) {
        if (line_number == 0) return;
        if (!(get_line(line_number - 1).size() == _max_line_size)) {
            if (is_end_line(line_number) && (line_number != _buffer.get_lines().size() - 1)) {
                if (get_line(line_number + 1).size() + get_line(line_number).size() <= _max_line_size) {
                    std::get<1>(_buffer.get_lines().at(line_number - 1)) = true;
                }
            } 
            auto line = std::get<0>(_buffer.get_lines().at(line_number));
            _buffer.get_lines().erase(_buffer.get_lines().begin() + line_number);
            insert_symbols(get_line(line_number - 1).size() + _left_margin, line_number - 1, line);
        }
    } else {
        auto erase_iter = std::get<0>(_buffer.get_lines().at(line_number)).begin() + x_curser - _left_margin - 1;
        std::get<0>(_buffer.get_lines().at(line_number)).erase(erase_iter);
        if (is_end_line(line_number) == false) { // append the first letter of next line
            auto next_line_symbol = std::get<0>(_buffer.get_lines().at(line_number + 1)).front(); 
            std::get<0>(_buffer.get_lines().at(line_number)) += next_line_symbol;
            delete_symbol(_left_margin + 1, line_number + 1);
            if (get_line(line_number + 1).size() == 0) {
                _buffer.get_lines().erase(_buffer.get_lines().begin() + line_number + 1);
                std::get<1>(_buffer.get_lines().at(line_number)) = true;
            }
        }
    }
}

void file::delete_symbols(int x_start, int y_start, int x_end, int y_end)
{
    if (y_end - y_start > 1) {
        auto erase_start = _buffer.get_lines().begin() + y_start + 1;
        auto erase_end = _buffer.get_lines().begin() + y_end;
        _buffer.get_lines().erase(erase_start, erase_end);
    }

    auto delete_substring = [this](int start_x, int end_x, int line_number) {
        auto& line = std::get<0>(_buffer.get_lines().at(line_number));
        line.erase(line.begin() + start_x, line.begin() + end_x);
        if (!is_end_line(line_number)) {
            std::get<1>(_buffer.get_lines().at(line_number)) = true;
            delete_symbol(_left_margin, line_number + 1);
        }
    };

    if (y_start == y_end) {
        if (x_start == 0 && x_end == std::get<0>(_buffer.get_lines().at(y_start)).size()) {
            auto erase_start = _buffer.get_lines().begin() + y_start;
            _buffer.get_lines().erase(erase_start);
   
        } else delete_substring(x_start, x_end, y_start);
    } else {
        delete_substring(x_start, std::get<0>(_buffer.get_lines().at(y_start)).size(), y_start);
        auto erase_start = _buffer.get_lines().begin() + y_start + 1;
        _buffer.get_lines().erase(erase_start);
    }
}

std::wstring file::get_line(int line_number)
{
    assert(line_number <= _buffer.get_lines().size());
    return _buffer.get_line(line_number);
}

bool file::is_end_line(int line_number)
{
    assert(line_number <= _buffer.get_lines().size());
    return std::get<1>(_buffer.get_lines().at(line_number));
}

int file::get_number_of_lines()
{
    return _buffer.get_lines().size();
}

int file::next_word_index(int x_index, int line_number, int word_count)
{
    auto line = get_line(line_number);
    if (x_index >= line.size() || line.size() == 0) return 0;
    
    for (; x_index < line.size(); ++x_index) {
        if (word_count == 0) return x_index;
        if (int(line.at(x_index)) == 160 || int(line.at(x_index)) == 95) --word_count;  
        if (int(line.at(x_index)) == 32) {
            if (x_index < line.size() - 1) {
                if (int(line.at(x_index + 1)) != 32) --word_count;
            } else {
                --word_count;
            }
        }
    }
    return x_index;
}

int file::prev_word_index(int x_index, int line_number, int word_count)
{
    auto line = get_line(line_number);

    if (line.size() == 0) return 0;
    if (x_index == line.size()) return --x_index;
    
    for (; x_index >= 0; --x_index) {
        if (word_count == 0) return x_index;
        if (int(line.at(x_index)) == 160 || int(line.at(x_index)) == 95) --word_count; 
        if (int(line.at(x_index)) == 32) {
            if (x_index > 0) {
                if (int(line.at(x_index - 1)) != 32) --word_count;
            } else {
                --word_count;
            }
        }
    }
    return x_index;
}

int file::resize(int current_line_number)
{
    auto find_empty_bottom_lines = [this]() {
        int empty_bottom_lines = 0;
        for (auto _ = _buffer.get_lines().end(); std::get<0>(*_).size() != 0; --_) {
            ++empty_bottom_lines;
        }    
        return empty_bottom_lines;
    };
    int empty_bottom_lines = find_empty_bottom_lines(); 

    _max_line_size = COLS - _left_margin - 1;
    auto lines = std::vector<std::tuple<std::wstring, bool> >{};
    lines.push_back(std::make_tuple(std::wstring{}, true));
    lines.reserve(_buffer.get_lines().size());
    int x_index = 0;
    for (auto line : _buffer.get_lines()) { 
        for (int i = 0; i < std::get<0>(line).size(); ++i) {
            if (x_index == _max_line_size) {
                std::get<1>(lines.back()) = false;
                lines.push_back(std::make_tuple(std::wstring{}, true));
                x_index = 0;
            }
            std::wstring s;
            s += std::get<0>(line).at(i);
            wchar_t wide_char[] = { s.at(0), 0x00 };
            std::get<0>(lines.back()) += wide_char; 
            ++x_index;
        }
        if (std::get<1>(line) == true) {
            std::get<1>(lines.back()) = true;
            lines.push_back(std::make_tuple(std::wstring{}, true));
            x_index = 0;
        }
    }
 
    int empty_bottom_lines_diff = find_empty_bottom_lines() - empty_bottom_lines;
    lines.erase(lines.end() - 1 - empty_bottom_lines_diff, lines.end());
    _buffer.get_lines() = lines;
    return current_line_number;
}
