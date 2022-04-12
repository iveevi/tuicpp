# tuicpp

tuicpp is a C++ wrapper API for ncurses which eases the developement of text
based interfaces and applications.

All API elements are enclosed inside the `tuicpp` namespace.

## Table of Contents

   * [Overview of tuicpp](#overview-of-tuicpp)
      * [Setting up](#setting-up)
      * [Import Structures](#import-structures)
         * [ScreenInfo](#screeninfo)
         * [World](#world)
      * [Window types](#window-types)
         * [PlainWindow](#plainwindow)
            * [Method Summary](#method-summary)
         * [BoxedWindow](#boxedwindow)
         * [DecoratedWindow](#decoratedwindow)

Created by [gh-md-toc](https://github.com/ekalinin/github-markdown-toc)

## Overview of tuicpp

### Setting up

The classes in `tuicpp` can be used almost immediately, however ncurses must
actually be initialized with `initscr()`. Likewise, it must be manually ended
with `endwin()` to prevent it from screwing with your terminal.

```cpp
// Example main
int main()
{
	// Initialize ncurses
	initscr();

	// tuicpp stuff goes here...

	// End ncurses
	// 	don't worry about the ncurses window
	//	handles, they are managed by tuicpp's
	//	windows
	endwin();
}
```

### Import Structures

#### ScreenInfo

The `ScreenInfo` struct is defined as follows.

```cpp
// Screen info
struct ScreenInfo {
        int height;
        int width;
        int y;
        int x;
};
```

It describes the position and size of the window. Note that `height` is before
`width` and `y` is before `x`. To avoid confusions with this altogether,
initialize these objects as follows:

```cpp
auto si = ScreenInfo {
	.height = 20,
	.width = 20,
	.y = 0,		// 0 is default
	.x = 0		// 0 is default
};
```

#### World

The `World` class does not represent an ncurses window, only the essential
members that all derived `Window` types will have.

A method to note from this class is the static `World::limits()` which returns a
`std::pair <int, int>` of the terminal's maximum height and width (note this
order).

### Window types

Now for the exciting stuff. Each section will show a snippet of code
demonstrating the construction and usage of the window and an animation of the
corresponding execution.

#### PlainWindow

As plain as a window can get.

```cpp
auto win = tuicpp::PlainWindow(
	tuicpp::ScreenInfo {
		.height = height,
		.width = width,
		.y = y,
		.x = x
	}
);

win.printf("Hello World!\n");
win.printf("This is just an ordinary window!\n");
win.printf("Notice that the text is being wrapped!\n");
win.printf("Press any key to continue...\n");
win.getc();
```

Result:

![](media/plain_window.png)

This class is important because it contains a conventional set of methods that
are inherited by all the remaining window types.

##### Method Summary

Method							| Description
---							| ---
`refresh()`						| Refreshes the window, essentially doing `wrefresh()`.
`clear()`						| Clears the window, essentially doing `wclear()`.
`erase()`						| Erase the window, essentially doing `werases()`. Unlike `clear()` there should not be as much flickering. All derived classes should this method as necessary.
`resize(int height, int width)`				| Resizes the window. Note the order of the arguments.
`move(int y, int x)`					| Moves the ***cursor*** to the yth row and xth column.
`printf(const char *fmt, ...)`				| Prints to the window, like `wprintw`.
`mvprintf(int y, int x, const char *fmt, ...)`		| Prints to the window starting at the yth row and xth column, like `mvwprintw`.
`add_char(const chtype ch)`				| Prints a characetr to the the window, like `waddch`.
`mvadd_char(int y, int x, const chtype ch)`		| Prints a characetr to the the window at the yth row and xth column, like `mvwaddch`.
`getc()`						| Get a character that the user pressed, like `wgetc()`. Will block until a key is pressed.
`set_keypad(bool bl)`					| Enables keypad for the window.
`cursor(int y, int x)`					| A more sensible alias for `move()`.
`attribute_on(int attr)`				| Turn on the attribute `attr` for the window, like `wattron()`.
`attribute_off(int attr)`				| Turn off the attribute `attr` for the window, like `wattroff()`.
`attribute_set(int attr)`				| Set the attribute for the window to `attr`, like `wattrset()`.

Some of these methods (i.e. `refresh()` and `clear()`) are overriden in derived
classes.

#### BoxedWindow

A window with a border surrounding it.

```cpp
auto win = tuicpp::BoxedWindow(
	tuicpp::ScreenInfo {
		.height = height,
		.width = width,
		.y = y,
		.x = x
	}
);

win.printf("Hello World!\n");
win.printf("This is no ordinary window ... it has a border!\n");
win.printf("Text is still being wrapped...\n");
win.printf("Press any key to continue...\n");
win.getc();
```

Result:

![](media/boxed_window.png)

#### DecoratedWindow

A window with a border and a title bar.

```cpp
auto win = tuicpp::DecoratedWindow(
	"My Decorated Window",
	tuicpp::ScreenInfo {
		.height = height,
		.width = width,
		.y = y,
		.x = x
	}
);

win.printf("Hello World!\n");
win.printf("This is a decorated window\n");
win.printf("It has a title bar and a border\n");
win.printf("Press any key to continue...\n");
win.getc();
```

Result:

![](media/decorated_window.png)

Now we get into more niche window types.

#### SelectionWindow

A window which handles the selection from a set of options. The constructor for
this class requires passing a `struct` of type `SelectionWindow::Option` to
specify certain options.

```cpp
auto opts = tuicpp::SelectionWindow {
	// Dictates whether the options should
	//	be center within the window
	.centered = true,

	// Allows multiple options to be selected
	//	(and then be confirmed by an [OK] button)
	.multi = false
};
```

Construction is fairly simple:

```cpp
// List of options -- OptionList is just an alias
//	for std::vector <std::string>
auto opt_list = tuicpp::SelectionWindow {
	"Option A",
	"Option B",
	"Option C",
	"Option D"
};

// As will be shown later, it should be more
//	common to allocate the window on heap,
//	so that it can be smoothly removed from
//	the screen when it is done
auto win = new tuicpp::SelectionWindow(
	"Selection Window",
	screen_info,
	opt_list,
	opts
);
```

The options that the user selects will be stored as indices to the `OptionList`
in a `SelectionWindow::Selection` object (an alias for `std::set <int>`).

```cpp
auto selection = tuicpp::SelectionWindow::Selection {};

// The return of this method is whether the
//	user has selected at least one option
//	(i.e. selection.size() > 0)
bool yeilded = win->yield(selection);

// Delete the window now that we are done with it
delete win;

// Whatever else we want to do...
```

The result of this is the following: [TODO]

With `opts.multi = true` it would instead look this: [TODO]

#### Table

The `Table` class is a templated interface designed to conveniently display a
table in ncurses. Its construction requires an auxiliarly structure, `Table <T>
::From`, which describes the columns of the table and how to generate them.

```cpp
// This lambda describes how to generate the column
//	entries for the table for one data input
//	(float, in our case)
auto to_str = [](const float &i, size_t column) {
	if (column == 0) {
		// First column is x
		return std::to_string(i);
	} else {
		// Second column is x * x
		return std::to_string(i * i);
	}
};

auto from = auto from = tuicpp::Table <float> ::From({"x", "f(x)"}, to_str);
```

To fill in the actual data for the table, one must assign the `.data` field (a
`std::vector` of the template parameter).

```cpp
from.data = {
	0, 0.5, 1.0, 1.5, 2.0, 2.5,
	3.0, 3.5, 4.0, 4.5, 5.0
};
```

Now, construction is simply as follows.

```cpp
auto win = tuicpp::Table <float> (
	from,
	tuicpp::ScreenInfo {
		.height = height,
		.width = width,
		.y = y,
		.x = x
	}
);
```

The result is the following:

![](media/table_window.png)

Additional options can be supplied to the table through the `From` structure.
The `.length` member (`std::vector <size_t>`) specifies the width of each table
column, and the boolean `.auto_resize` dictates whether the `Table` object's
window will be resized to fit the entire table.

The `Table` class also comes with the following methods.

Method							| Description
---							| ---
`set_data(const Data &data, bool auto_resize = false)`	| Changes the table's data to `data`. If `auto_resize` is set to `true`, then the window will resize to fit the whole table.
`set_lengths(const Lengths &lengths)`			| Sets the width of each column.
`set_generator(const Generator &generator)`		| Changes the column generator function to `generator`. The expected signature for `Generator` is `std::string (const T &, size)`.
`highlight_row(int row)`				| Highlight's a specific row in the table.


#### FieldEditor

Edit fields in a slick little window. Construction is very easy:

```cpp
// We allocate this on the heap to remove it from
//	our app seamlessly
auto win = new tuicpp::FieldEditor(
	"Employee Editor",
	{"Name", "Email"},		// Names of the fields
	tuicpp::ScreenInfo {
		.height = height,
		.width = width,
		.y = y,
		.x = x
	}
);
```

This defines an editor with the fields "Name" and "Email". To get user input
(and display the editor), one does the following:

```cpp
// Assign the initial values of the fields
std::string name = "Bob Joe";
std::string email;

// "Yield" the user input
win->yield({
	tuicpp::yielder(&name),
	tuicpp::yielder(&email)
});

// Done with the editor, we can free it now
delete win;

// Do whatever else...
```

The `yield` method takes some pretty weird objects. For now, think of
`yielder(&name)` as a wrapper to the reference to `name`. When the editor is
done with the `yield` method, the contents of the fields will the stored in
`name` and `email`. No extra hassle.

The result of this setup is the following.

![](media/editor_window.gif)
