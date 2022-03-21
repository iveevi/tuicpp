#ifndef TUICPP_H_
#define TUICPP_H_

// Standard headers
#include <functional>
#include <memory>
#include <string>
#include <vector>

// Ncurses
#include <ncurses.h>

namespace tuicpp {

///////////////////////////
// Main window hierarchy //
///////////////////////////

// Screen info
struct ScreenInfo {
        int height;
        int width;
        int y;
        int x;
};

// Generic window class
class Window {
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

	// Get max height and width
	static std::pair <int, int> limits() {
		int max_height, max_width;
		getmaxyx(stdscr, max_height, max_width);
		return std::make_pair(max_height, max_width);
	}
};

// Plain window, no border
class PlainWindow : public Window {
protected:
	WINDOW *_main = nullptr;

	// TODO: do we need subwindows?
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
		werase(_main);
		wrefresh(_main);
		delwin(_main);
	}

	// Refreshing
	virtual void refresh() const {
		wrefresh(_main);
	}

	// Clear screen
	virtual void clear() const {
		wclear(_main);
	}

	// Erase screen
	virtual void erase() const {
		werase(_main);
	}

	// Resizing window
	virtual void resize(int height, int width) const {
		wresize(_main, height, width);
	}

	// Move cursor to position
	virtual void move(int y, int x) const {
		wmove(_main, y, x);
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

	void attribute_set(int attr) {
		wattrset(_main, attr);
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
	// TODO: clean up (duplicated code)
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
		werase(_box);
		wrefresh(_box);
		delwin(_box);
	}
};

// Decorated Window (title, border, etc.)
class DecoratedWindow : public BoxedWindow {
protected:
        WINDOW *_title = nullptr;
	const std::string _title_str;
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
		werase(_title);
		wrefresh(_title);
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
	// TODO: how to get the content inside a window?

	// Fixed distances
	static constexpr int decoration_height = 5;
};

//////////////////////////////
// Specialized window types //
//////////////////////////////

template <class T>
class Table : public PlainWindow {
public:
	// Aliases
	using Headers = std::vector <std::string>;
	using Data = std::vector <T>;
	using Generator = std::function <std::string (const T &, size_t)>;
	using Lengths = std::vector <size_t>;

	// Update structure
	struct From {
		Headers headers;
		Data data;
		Generator generator;
		Lengths lengths;

		bool auto_resize = false;

		// Constructor from headers and generator
		From(const Headers &headers, Generator generator)
				: headers(headers), generator(generator) {}
	};
protected:
	Headers _headers;
	Data _data;
	Lengths _lengths;

	// TODO: array of lengths for each column (either auto or specified)

	// Function to generate columns from data
	Generator _generator;

	// Get lengths for each column
	void _get_lengths() {
		_lengths = Lengths(_headers.size(), 0);

		for (size_t i = 0; i < _headers.size(); i++) {
			_lengths[i] = _headers[i].length();
			for (const auto &d : _data) {
				size_t l = _generator(d, i).length();
				if (l > _lengths[i])
					_lengths[i] = l;
			}
		}
	}

	// Write table
	void _write_table(int highlight = -1) const {
		// Variables
		int x = 0;
		int line = 0;

		// Write top bar
		x = 0;

		mvadd_char(line, 0, ACS_ULCORNER);
		for (size_t i = 0; i < _headers.size(); i++) {
			for (int j = 0; j < _lengths[i] + 2; j++)
				mvadd_char(line, x + j + 1, ACS_HLINE);
			x += _lengths[i] + 3;

			if (i != _headers.size() - 1)
				mvadd_char(line, x, ACS_TTEE);
			else
				mvadd_char(line, x, ACS_URCORNER);
		}
		line++;

		// Write headers
		x = 1;
		for (size_t i = 0; i < _headers.size(); i++) {
			mvprintf(line, x, " %s ", _headers[i].c_str());
			x += _lengths[i] + 3;
			mvadd_char(line, x - 1, ACS_VLINE);
		}
		mvadd_char(line, 0, ACS_VLINE);
		line++;

		// Write middle bar
		x = 0;

		mvadd_char(line, 0, ACS_LTEE);
		for (size_t i = 0; i < _headers.size(); i++) {
			for (int j = 0; j < _lengths[i] + 2; j++)
				mvadd_char(line, x + j + 1, ACS_HLINE);
			x += _lengths[i] + 3;

			if (i != _headers.size() - 1)
				mvadd_char(line, x, ACS_PLUS);
			else
				mvadd_char(line, x, ACS_RTEE);
		}
		line++;

		// Write data
		for (size_t n = 0; n < _data.size(); n++) {
			x = 1;

			T d = _data[n];
			for (size_t i = 0; i < _headers.size(); i++) {
				std::string str = _generator(d, i);
				
				// Pad string with spaces
				str = str.substr(0, _lengths[i]);
				if (str.length() < _lengths[i])
					str.append(std::string(_lengths[i] - str.length(), ' '));

				// Highlight if needed
				if (n == highlight)
					wattrset(_main, A_REVERSE);
				
				mvprintf(line, x, " %s ", str.c_str());

				if (n == highlight)
					wattrset(_main, A_NORMAL);

				// Normal
				x += _lengths[i] + 3;
				mvadd_char(line, x - 1, ACS_VLINE);
			}
			mvadd_char(line, 0, ACS_VLINE);
			line++;
		}

		// Write the bottom bar
		x = 0;

		mvadd_char(line, 0, ACS_LLCORNER);
		for (size_t i = 0; i < _headers.size(); i++) {
			for (int j = 0; j < _lengths[i] + 2; j++)
				mvadd_char(line, x + j + 1, ACS_HLINE);
			x += _lengths[i] + 3;

			if (i != _headers.size() - 1)
				mvadd_char(line, x, ACS_BTEE);
			else
				mvadd_char(line, x, ACS_LRCORNER);
		}
		line++;
	}
public:
	// Default constructor
	Table() = default;

	// Constructors
	// TODO: auto resize window
	Table(const From &from, int height, int width, int y, int x)
			: PlainWindow(height, width, y, x),
			_headers(from.headers), _data(from.data),
			_generator(from.generator), _lengths(from.lengths) {
		// Get lengths (auto)
		if (_lengths.empty())
			_get_lengths();

		// Resize window if requested
		if (from.auto_resize) {
			int new_height = _data.size() + 4;
			int new_width = 1;
			for (const auto &l : _lengths)
				new_width += l + 3;
			resize(new_height, new_width);
		}

		// Write table
		_write_table();

		// Refresh all boxes
		wrefresh(_main);
	}

	Table(const From &from, const ScreenInfo &info)
			: Table(from,
				info.height, info.width,
				info.y, info.x
			) {}

	// Update data
	void set_data(const Data &data, bool auto_resize = false) {
		// First, erase
		erase();

		_data = data;

		if (auto_resize) {
			_lengths.clear();
			_get_lengths();
			resize(_data.size() + 4, 1);
		}

		_write_table();
		wrefresh(_main);
	}

	// Update lengths
	void set_lengths(const Lengths &lengths) {
		// First, erase
		erase();

		_lengths = lengths;
		_write_table();
		wrefresh(_main);
	}

	// Set the generator
	void set_generator(const Generator &generator) {
		// First, erase
		erase();

		_generator = generator;
		_write_table();
		wrefresh(_main);
	}

	// Highlight a row
	void highlight_row(int row) {
		// First, erase
		erase();
		_write_table(row);
		wrefresh(_main);
	}
};

// Field editor window
template <class ... Args>
class FieldEditor : public DecoratedWindow {
public:
	// Aliases
	using Fields = std::array <std::string, sizeof...(Args)>;
	using Yield = std::tuple <Args...>;
protected:
	Fields _fields;
public:
	// Default constructor
	FieldEditor() = default;

	// Constructor
	FieldEditor(const std::string &title, const Fields &fnames,
			const ScreenInfo &info)
			: DecoratedWindow(title, info), _fields(fnames) {
		// Write the fields
		int line = 0;
		for (const auto &f : _fields) {
			mvprintf(line, 0, "%s: ", f.c_str());
			line++;
		}

		// Add an [Enter] button
		mvprintf(line, 0, "[Enter]");
	}

	// Yield the fields
	Yield yield() {
		// Tuple to return
		Yield ret;

		// Field index
		int field = 0;

		// Enable cursors and echo
		curs_set(1);
		echo();

		// Get the fields
		int c;
		while ((c = getc())) {
		}

		// Disable cursors and no echo
		curs_set(0);
		noecho();
	}
};

}

#endif
