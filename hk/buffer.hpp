#ifndef __BUFFER__HPP__
#define __BUFFER__HPP__


#include <vector>
#include <string>
#include <tuple>

class buffer {
public:
    std::vector<std::tuple<std::wstring, bool> >& get_buffer();
    int get_number_of_lines();
    std::wstring get_line(int line_number);
private:
    std::vector<std::tuple<std::wstring, bool> > _lines = std::vector<std::tuple<std::wstring, bool> >{};
};

#endif // __BUFFER__HPP__
