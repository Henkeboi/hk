#ifndef __TEXT__BUFFER__HPP__
#define __TEXT__BUFFER__HPP__

#include <vector>
#include <string>
#include <tuple>

class buffer {
public:
    std::vector<std::tuple<std::wstring, bool> >& get_lines();
    int get_number_of_lines();
    std::wstring get_line(int line_number);
    void clear_text();
private:
    std::vector<std::tuple<std::wstring, bool> > _lines = std::vector<std::tuple<std::wstring, bool> >{};
};

#endif // __TEXT__BUFFER__HPP__
