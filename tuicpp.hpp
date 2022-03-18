#ifndef TUICPP_H_
#define TUICPP_H_

// Standard headers
#include <memory>
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
	// Subwindows
	std::vector <std::shared_ptr <Window>> _subwindows;
public:
	ScreenInfo info;

	// Default constructor
	Window() = default;

	// Constructors
	Window(int height, int width, int y, int x)
			: info {height, width, y, x} {}
	Window(const ScreenInfo &i)
			: info {i} {}

	// Destructor
	virtual ~Window() = default;
};

// TODO: plainwindow class, which has the subwindows as children
// Plain window, no border
class PlainWindow : public Window {
protected:
	WINDOW *_main	= nullptr;
public:
	// Default constructor
	PlainWindow() = default;

	// Constructors
	PlainWindow(int height, int width, int y, int x)
			: Window(height, width, y, x) {
		// Create the windows
		_main = newwin(height, width, y, x);
	}

	PlainWindow(const ScreenInfo &i)
			: Window(i) {
		// Create the windows
		_main = newwin(info.height, info.width, info.y, info.x);
	}

	// Destructor
	virtual ~PlainWindow() {
		delwin(_main);
	}
	
	// Refreshing
	virtual void refresh() const {
		wrefresh(_main);
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

	// Adding characters
	void add_char(const chtype ch) const {
		waddch(_main, ch);
		wrefresh(_main);
	}

	void mvadd_char(int y, int x, const chtype ch) const {
		mvwaddch(_main, y, x, ch);
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
};

// Window with a boxed border
class BoxedWindow : public PlainWindow {
protected:
        WINDOW *_box    = nullptr;
	// WINDOW *_main	= nullptr;
public:
	// Default constructor
	BoxedWindow() = default;

	// Constructors
	BoxedWindow(int height, int width, int y, int x)
			: PlainWindow(height, width, y, x) {
		// Create the windows
		_box = newwin(height, width, y, x);
		_main = newwin(height - 2, width - 2, y + 1, x + 1);

		// Borders
		box(_box, 0, 0);

		// Refresh all boxes
		wrefresh(_box);
	}

	BoxedWindow(const ScreenInfo &i)
			: PlainWindow(i) {
		// Create the windows
		_box = newwin(info.height, info.width, info.y, info.x);
		_main = newwin(info.height - 2, info.width - 2, info.y + 1, info.x + 1);

		// Borders
		box(_box, 0, 0);

		// Refresh all boxes
		wrefresh(_box);
	}

	// Destructor
	virtual ~BoxedWindow() {
		// Delete the windows
		delwin(_box);
	}
};

// Decorated Window (title, border, etc.)
class DecoratedWindow : public BoxedWindow {
protected:
        WINDOW			*_title = nullptr;
	const std::string	_title_str;
public:
	// Default constructor
	DecoratedWindow() = default;

	// Constructors
	DecoratedWindow(const std::string &title, int height, int width, int y, int x)
			: BoxedWindow(height, width, y, x), _title_str(title) {
		// Create the windows
		_main = newwin(height - 5, width - 2, y + 4, x + 1);
		_title = newwin(3, width - 2, y + 1, x + 1);

		// Borders
		box(_title, 0, 0);

		// Write title
		int remaining = (width - 2) - title.length();
		mvwprintw(_title, 1, remaining/2, "%s", title.c_str());

		// Refresh all boxes
		wrefresh(_title);
	}

	DecoratedWindow(const std::string &title, const ScreenInfo &info)
			: DecoratedWindow(title, info.height, info.width, info.y, info.x) {}

	// Destructor
	virtual ~DecoratedWindow() {
		// Delete the windows
		delwin(_title);
	}

	// Refreshing
	virtual void refresh() const override {
		BoxedWindow::refresh();
		wrefresh(_title);
	}

	// Give title text an attribute
	void attr_title(int attr) {
		wattron(_title, attr);
		
		int remaining = (info.width - 2) - _title_str.length();
		mvwprintw(_title, 1, remaining/2, "%s", _title_str.c_str());
		
		wattroff(_title, attr);
		wrefresh(_title);
	}

	// TODO: change title string (with option to autoresize)
	// + resize function

	// Fixed distances
	static constexpr int decoration_height = 5;
};

}

#endif
