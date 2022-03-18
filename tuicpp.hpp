#ifndef TUICPP_H_
#define TUICPP_H_

// Standard headers
#include <string>

// Ncurses
#include <ncurses.h>

namespace tuicpp {

// Screen info
struct ScreenInfo {
        int height;
        int width;
        int y;
        int x;
};

// Generic window class
class Window {
protected:
	ScreenInfo _info;
public:
	Window() {}
	Window(int height, int width, int y, int x)
			: _info {height, width, y, x} {}
	Window(const ScreenInfo& info)
			: _info {info} {}
};

// Boxed window (title, border, etc.)
// TODO: refactor this class (it has more than a box)
class BoxedWindow : public Window {
        WINDOW *_box    = nullptr;
        WINDOW *_main   = nullptr;
        WINDOW *_title  = nullptr;

	// TODO: subwindows
public:
	BoxedWindow() {}

	BoxedWindow(int height, int width, int y, int x)
			: Window(height, width, y, x) {
		// Create the windows
		_box = newwin(height, width, y, x);
		_main = newwin(height - 2, width - 2, y + 1, x + 1);

		// Borders
		box(_box, 0, 0);

		// Refresh all boxes
		wrefresh(_box);
	}

	BoxedWindow(const std::string &title, int height, int width, int y, int x)
			: Window(height, width, y, x) {
		// Create the windows
		_box = newwin(height, width, y, x);
		_main = newwin(height - 5, width - 2, y + 4, x + 1);
		_title = newwin(3, width - 2, y + 1, x + 1);

		// Borders
		box(_box, 0, 0);
		box(_title, 0, 0);

		// Write title
		int remaining = (width - 2) - title.length();
		mvwprintw(_title, 1, remaining/2, "%s", title.c_str());

		// Refresh all boxes
		wrefresh(_box);
		wrefresh(_title);
	}

	BoxedWindow(const std::string &title, const ScreenInfo &info)
			: BoxedWindow(title, info.height, info.width, info.y, info.x) {}

	// Refreshing
	virtual void refresh() const {
		wrefresh(_box);
		wrefresh(_main);
		wrefresh(_title);
	}

	// Printing
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

	// Interact
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

	// Attributes
	void attribute_on(int attr) {
		wattron(_main, attr);
	}

	void attribute_off(int attr) {
		wattroff(_main, attr);
	}
	
	// TODO: place subwindows

	// Fixed distances
	static constexpr int decoration_height = 5;
};

}

#endif
