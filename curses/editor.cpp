#include "editor.hpp"
#include "curses.h"

editor::editor(std::string filename)
: _filename(filename), _text_window(text_window{filename}), _mode(normal), _window(text), _mode_switch_timer(std::chrono::system_clock::now()), _repeat_timer(std::chrono::system_clock::now()), _repeat_string("-1"), _exit(false)
{}

bool editor::open_file(std::string filename)
{
    if (_text_window.open_file(filename)) {
        _text_window.display();
        _text_window.resize();
        _filename = filename;
        return true;
    }
    return false;
}

bool editor::save_file(std::string filename)
{
    if (_text_window.save_file(filename)) return true;
    return false;
}

void editor::edit()
{
    _text_window.display();
    bool j_symbol_not_inserted = false;
    while (!_exit) {
        mode next_mode = _mode;
        window next_window = _window;
        std::wstring command_string = L"";
        wint_t user_input = _get_input();
        if (user_input == KEY_RESIZE) {
            _text_window.resize();
        } else if (_window == text) {
            if (_mode == normal) {
                // repeat commands
                if (user_input == int('0') && _repeat_string == "-1") {
                    _text_window.curser_start_line();
                    _text_window.display();
                } else if (user_input == int('0')) {
                    _append_to_repeat_string('0');
                }  else if (user_input == int('1')) {
                    _append_to_repeat_string(user_input);
                } else if (user_input == int('2')) {
                    _append_to_repeat_string(user_input);
                } else if (user_input == int('3')) {
                    _append_to_repeat_string(user_input);
                } else if (user_input == int('4')) {
                    _append_to_repeat_string(user_input);
                } else if (user_input == int('5')) {
                    _append_to_repeat_string(user_input);
                } else if (user_input == int('6')) {
                    _append_to_repeat_string(user_input);
                } else if (user_input == int('7')) {
                    _append_to_repeat_string(user_input);
                } else if (user_input == int('8')) {
                    _append_to_repeat_string(user_input);
                } else if (user_input == int('9')) {
                    _append_to_repeat_string(user_input);
                // move curser
                } else if (user_input == int('j')) {
                    _text_window.curser_down(_get_repeat_number());
                    _text_window.display();
                } else if (user_input == 4) { // CTRL + d
                    _text_window.curser_down(_get_repeat_number() * ((LINES - 2) / 2));
                    _text_window.display();
                } else if (user_input == int('j')) {
                    _text_window.curser_down(_get_repeat_number());
                    _text_window.display();
                } else if (user_input == int('k')) {
                    _text_window.curser_up(_get_repeat_number());
                    _text_window.display();
                } else if (user_input == 21) { // CTRL + u
                    _text_window.curser_up(_get_repeat_number() * 28);
                    _text_window.display();
                } else if (user_input == int('h')) {
                    _text_window.curser_left(_get_repeat_number());
                    _text_window.display();
                } else if (user_input == int('l')) {
                    _text_window.curser_right(_get_repeat_number());
                    _text_window.display();
                } else if (user_input == int('e')) {
                    _text_window.curser_next_word(_get_repeat_number());
                    _text_window.display();
                } else if (user_input == int('b')) {
                    _text_window.curser_prev_word(_get_repeat_number());
                    _text_window.display();
                } else if (user_input == int('$')) {
                    _text_window.curser_end_line();
                    _repeat_string = "-1";
                    _text_window.display();
                // move dispay
                } else if (user_input == int('x')) {
                    _text_window.display_center(); 
                } else if (user_input == int('z')) {
                    _text_window.display_up(_get_repeat_number());
                } else if (user_input == int('c')) {
                    _text_window.display_down(_get_repeat_number());
                // change to insert
                } else if (user_input == int('i')) {
                    next_mode = insert;
                } else if (user_input == int('o')) {
                    next_mode = insert;
                    _text_window.curser_end_line();
                    _text_window.insert_newline();
                    _text_window.auto_indent();
                    _repeat_string = "-1";
                    _text_window.display();
                } else if (user_input == int('O')) {
                    next_mode = insert;
                    _text_window.curser_start_line();
                    _text_window.insert_newline();
                    _text_window.curser_up(1);
                    _text_window.auto_indent();
                    _repeat_string = "-1";
                    _text_window.display();
                // visual operations
                } else if (user_input == int('v')) {
                    _text_window.toggle_select();
                } else if (user_input == int('d')) {
                    _text_window.delete_selection();
                    _text_window.display();
                // change to command
                } else if (user_input == int(':')) {
                    next_window = command;
                    next_mode = insert;
                    _command_window.insert(':');
                    _command_window.display();
                } else if (user_input == 0) { // CTRL + c
                    _command_window.print_abort_message();
                }
                
            } else if (_mode == insert) {
                // change to normal
                auto now = std::chrono::system_clock::now();
                auto time_span = std::chrono::duration_cast<std::chrono::duration<double> >(now - _mode_switch_timer);
                if (j_symbol_not_inserted && user_input != int('k')) {
                    _text_window.insert('j');
                    j_symbol_not_inserted = false;
                }
                if (user_input == int('j')) {
                    _mode_switch_timer = std::chrono::system_clock::now(); 
                    j_symbol_not_inserted = true;
                } else if (user_input == int('k')) {
                    if (time_span.count() < _timer_constant && j_symbol_not_inserted == true) {
                        next_mode = normal;
                    } else {
                        if (j_symbol_not_inserted) _text_window.insert('j');
                        _text_window.insert('k');
                    }
                    j_symbol_not_inserted = false;
                }
                if (user_input == 9) { // tab
                    std::wstring tab = L"    ";
                    _text_window.insert_symbols(tab);
                    _text_window.display();
                } else if (user_input == 127) { // delete
                    _text_window.delete_symbol();
                    _text_window.display();
                } else if (user_input == 10) { // newline
                    _text_window.insert_newline();
                    _text_window.auto_indent();
                    _text_window.display();
                } else if (user_input == 0) { // CTRL + c
                    _command_window.print_abort_message();
                } else if (user_input != int('j') && user_input != int('k')) { // normal insert
                    _text_window.insert(user_input);
                    _text_window.display();
                }
            }
        } else if (_window == command) {
            if (_mode == insert) {
                if (user_input == 10) { // newline
                    command_string = _command_window.execute();
                    next_window = text; 
                    next_mode = normal;
                    _text_window.display();
                } else {
                    _command_window.insert(user_input);
                    _command_window.display();
                }
            }
        }
        if (command_string != L"") {
            _execute(command_string);
            command_string = L"";
        }
        _mode = next_mode;
        _window = next_window;
    }
}

void editor::_append_to_repeat_string(char number)
{
    auto now = std::chrono::system_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::duration<double> >(now - _repeat_timer);
    if (time_span.count() < _timer_constant) {
        if (_repeat_string == "-1" && number != 0) {
            _repeat_string = number;
        } else {
            _repeat_string += number; 
        }
    } else {
        _repeat_string = "-1";
    }
    _repeat_timer = std::chrono::system_clock::now(); 
}

int editor::_get_repeat_number()
{
    int number = 1;
    auto  now = std::chrono::system_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::duration<double> >(now - _repeat_timer);

    if (time_span.count() < _timer_constant) {
        number = std::stoi(_repeat_string);
        if (number == -1) {
            number = 1;
        }
    }
    _repeat_string = "-1";
    _repeat_timer = std::chrono::system_clock::now(); 
    return number;
}

void editor::_execute(std::wstring command)
{
    if (command == L"q") {
        _exit = true;
    } else if (command == L"w") {
        _text_window.save_file(_filename);
    } else if (command == L"wq") {
        _text_window.save_file(_filename);
        _exit = true;
    }
}

wint_t editor::_get_input()
{
    wint_t input_symbol;
    get_wch(&input_symbol);
    return input_symbol;
}
