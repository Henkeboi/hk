CXXFLAG= -std=c++1z -Wall -Wextra -g -Wunused -vv
LIBS=-lncursesw


ed: ed.cpp editor.o command_window.o text_buffer.o file.o text_window.o
	g++ $(CXXFLAGS) ed.cpp editor.o command_window.o text_buffer.o text_window.o file.o $(LIBS) -o ed 

editor.o: editor.cpp editor.hpp
	g++ $(CXXFLAGS) -c editor.cpp $(LIBS)

command_window.o: command_window.cpp command_window.hpp
	g++ $(CXXFLAGS) -c command_window.cpp $(LIBS)

text_buffer.o: text_buffer.cpp text_buffer.hpp
	g++ $(CXXFLAGS) -c text_buffer.cpp $(LIBS)

file.o: file.cpp file.hpp
	g++ $(CXXFLAGS) -c file.cpp $(LIBS)

text_window.o: text_window.cpp text_window.hpp
	g++ $(CXXFLAGS) -c text_window.cpp $(LIBS)


clean:
	rm *.o *~ ed

install:
	make
	chmod 755 ed
	sudo cp ed /usr/local/bin/
	



