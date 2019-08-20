#ifndef __EDITOR__HPP__
#define __EDITOR__HPP__

#include <chrono>
#include <string>
#include "text_window.hpp"
#include "command_window.hpp"

class editor {
public:
    editor() = delete;
    editor(std::string filename);
    void edit();
    bool open_file(std::string filename);
    bool save_file(std::string filename);
private:
    void _append_to_repeat_string(char number);
    int _get_repeat_number();
    text_window _text_window;
    command_window _command_window;
    enum mode { normal=0, insert=1 } _mode;
    enum window { text=0, command=1 } _window;
    bool _exit;
    std::chrono::system_clock::time_point _mode_switch_timer;    
    std::chrono::system_clock::time_point _repeat_timer;
    std::string _repeat_string;
    std::string _filename;
    const double _timer_constant = 2;
    void _execute(std::wstring command);
    wint_t _get_input();
};

#endif // __EDITOR__HPP__
 
