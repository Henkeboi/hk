#include "command_window.hpp"
#include "cassert"
#include "curses.h"

command_window::command_window()
: _command_string(L""), _x_curser(0)
{}

void command_window::display()
{
    move(LINES - 1, 0);
    for (int x_printer = 0; x_printer < _command_string.size(); ++x_printer) {
        std::wstring s;
        wint_t symbol = _command_string.at(x_printer);
        s += symbol;
        wchar_t wide_char[] = { s.at(0), 0x00 };
        addwstr(wide_char);
    }
}

void command_window::insert(wint_t symbol)
{
    _command_string += symbol;
}


std::wstring command_window::execute()
{
    auto command = _command_string.substr(1);
    _command_string = L"";
    return command;
}

void command_window::print_abort_message()
{
    move(LINES - 1, 0);
    addstr(":q to exit");
}
