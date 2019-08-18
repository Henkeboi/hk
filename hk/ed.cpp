#include <iostream>
#include <fstream>
#include <locale>
#include <string>
#include <sstream>
#include <cstdlib>
#include "signal.h"
#include "unistd.h"
#include "curses.h"

#include "editor.hpp"


void abort_handler(int s) {}

int main(int argc, char* argv[])
{
    initscr();
    cbreak();
    noecho();
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    struct sigaction sig_handler;
    sig_handler.sa_handler = abort_handler;
    sigemptyset(&sig_handler.sa_mask);
    sig_handler.sa_flags = 0;
    sigaction(SIGINT, &sig_handler, NULL);

    try {
        if (!setlocale(LC_ALL, "")) {
            std::cout << "Couldnt set locale" << "\n";
        } 
        editor ed = editor(argv[1]);
        ed.edit();
        std::system("stty sane");
    } catch (...) {
        std::system("stty sane");
    }

    endwin();
    return 0;
}
