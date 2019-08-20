#include "text_buffer.hpp"


std::vector<std::tuple<std::wstring, bool> >& buffer::get_lines()
{
    return _lines;
}

std::wstring buffer::get_line(int line_number)
{
    return std::get<0>(_lines.at(line_number));
}

int buffer::get_number_of_lines()
{
    return _lines.size();
}

void buffer::clear_text()
{
    _lines.clear();
}
