#ifndef __COMMAND__WINDOW__HPP__
#define __COMMAND__WINDOW__HPP__

#include <vector>
#include <string>

class command_window {
public:
    command_window(); 
    void display();
    void insert(wint_t symbol);
    void delete_symbol();
    std::wstring execute();
    void print_abort_message();
private:
    std::wstring _command_string;
    int _x_curser;
};

#endif // __COMMAND__WINDOW__HPP__
