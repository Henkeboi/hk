#include <fstream>
#include "curses.h"
#include "cstdlib"
#include <iostream>
#include <locale>
#include "signal.h"


void abort_handler(int s) {
    printf("%d\n", s);
}

int main()
{
    try {
        if (!setlocale(LC_ALL, "")) {
            std::cout << "Couldnt set locale" << "\n";
        }
        initscr();
        cbreak();
        noecho();

        struct sigaction sig_handler;
        sig_handler.sa_handler = abort_handler;
        sigemptyset(&sig_handler.sa_mask);
        sig_handler.sa_flags = 0;
        sigaction(SIGINT, &sig_handler, NULL);


        // Mappings
        nl();
        wint_t a;
        get_wch(&a);
        std::ofstream file;
        file.open("mappings", std::wios::out | std::wios::in);
        file << a << '\n';
        file.close();
        endwin();
        std::system("stty sane");
    } catch (...) {
        std::system("stty sane");
    }
    return 0;
}
