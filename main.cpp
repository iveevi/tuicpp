#include <fstream>
#include <iostream>
#include <vector>

#include <ncurses.h>

#include "cpp.nabu.hpp"

// Screen info
struct Scrinfo {
        int height;
        int width;
        int y;
        int x;
};

// Window structures
class BoxedWindow {
        WINDOW *_box    = nullptr;
        WINDOW *_main   = nullptr;
        WINDOW *_title  = nullptr;
protected:
        Scrinfo _info;
public:
	BoxedWindow() {}

	BoxedWindow(int height, int width, int y, int x)
		: _info {height, width, y, x} {
		// Create the windows
		_box = newwin(height, width, y, x);
		_main = newwin(height - 2, width - 2, y + 1, x + 1);

		// Borders
		box(_box, 0, 0);

		// Refresh all boxes
		wrefresh(_box);
	}

	BoxedWindow(int height, int width, int y, int x, const std::string &title)
		: _info {height, width, y, x} {
		// Create the windows
		_box = newwin(height, width, y, x);
		_main = newwin(height - 5, width - 2, y + 4, x + 1);
		_title = newwin(3, width - 2, y + 1, x + 1);

		// Borders
		box(_box, 0, 0);
		box(_title, 0, 0);

		// Write title
		int remaining = (width - 2) - title.length();
		mvwprintw(_title, 1, remaining/2, title.c_str());

		// Refresh all boxes
		wrefresh(_box);
		wrefresh(_title);
	}

	virtual void refresh() const {
		wrefresh(_box);
		wrefresh(_main);
		wrefresh(_title);
	}

	template <typename ... Args>
		void printf(const char *str, Args ... args) const {
			wprintw(_main, str, args...);
			wrefresh(_main);
		}

	template <typename ... Args>
		void mvprintf(int y, int x, const char *str, Args ... args) const {
			mvwprintw(_main, y, x, str, args...);
			wrefresh(_main);
		}

	int getc() const {
		return wgetch(_main);
	}

	void set_keypad(bool bl) {
		// Set keypad options
		keypad(_main, bl);
	}

	void cursor(int y, int x) {
		wmove(_main, y, x);
	}
};

// Debugging window
Window debug;

// TODO: simple textbox class
class TextEditor : public Window {
        std::vector <std::string>       _lines;

        // Text pad
        WINDOW *                        _pad    = nullptr;

        // Top line number
        int                             _cline = 0;

        // Read file lines
        void read_file(const std::string &file) {
                std::string line;

                std::ifstream fin(file);
                while (getline(fin, line))
                        _lines.push_back(line);
        }
public:
        TextEditor(int height, int width, int y, int x, const std::string &file)
                        : Window(height, width, y, x, file) {
                // Read file lines
                read_file(file);
		
                // Colors
                init_pair(1, 132, COLOR_BLACK); // TODO: static method
                init_pair(2, 31, COLOR_BLACK);
		init_pair(3, 215, COLOR_BLACK);	// for line numbers
		// TODO: constants for colors

		// Create text pad and fill it out
                _pad = newpad(_lines.size(), width - 2);

                int ln = 0;
                int maxl = std::to_string(_lines.size()).length();

                nabu::StringFeeder sf = nabu::StringFeeder::from_file(file);

                nabu::ret rptr;
                
                // First line
		// TODO: method to set color
		wattron(_pad, COLOR_PAIR(3));
                wprintw(_pad, "%*d: ", maxl, ++ln);
		wattroff(_pad, COLOR_PAIR(3));

                while ((rptr = nabu::rule <cpp::sh_main> ::value(&sf))) {
                        nabu::mt_ret mr = nabu::get <nabu::mt_ret> (rptr);
                        std::string str = nabu::get <std::string> (mr.second);

                        if (str == "\n") {
                                ln++;
				wattron(_pad, COLOR_PAIR(3));
                                wprintw(_pad, "\n%*d: ", maxl, ++ln);
				wattroff(_pad, COLOR_PAIR(3));
                        } else {
                                wattron(_pad, COLOR_PAIR(mr.first));
                                wprintw(_pad, "%s", str.c_str());
                                wattroff(_pad, COLOR_PAIR(mr.first));
                        }
                }

                cursor(0, 0);
                
                // Set keypad to true
                set_keypad(true);
        }

        void refresh() const override {
                Window::refresh();
                prefresh(_pad, _cline, 0,
                        _info.y + 4,
                        _info.x + 1,
                        _info.y + _info.height - 2,
                        _info.x + _info.width - 2
                );
        }

        // Handle input
        void handle(int c) {
                debug.mvprintf(0, 0, "Got: %d vs Up=%d\n", c, KEY_UP);
                switch(c) {
                case KEY_UP:
                        if (_cline > 0)
                                _cline--;
                        break;
                case KEY_DOWN:
                        if ((_cline + _info.height - 5) < _lines.size())
                                _cline++;
                        break;
                default:
                        break;
                }

                // Move the cursor appropriately
                cursor(0, 0);

                // Refresh text pad
                refresh();
        }
};

int main()
{
        initscr();
        noecho();
        curs_set(0);
        start_color();

        /* Window window(15, 20, 0, 0, "Title");
        window.refresh();
        window.printf("Hello from box"); */
        debug = Window(15, 20, 0, 0, "Debug");
        debug.refresh();

        TextEditor te(20, 80, 10, 30, "main.cpp");
        te.refresh();
        
        int c;
        while ((c = te.getc())) {
                te.handle(c);
                if (c == 27)
                        break;
        }

        endwin();
        std::cout << "C = " << c << std::endl;
}
