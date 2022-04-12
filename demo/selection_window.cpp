#include "global.hpp"

// TODO: clean up the duplicate code with a common function
void selection_window()
{
	static int height = 10;
	static int width = 34;

	auto pr = tuicpp::Window::limits();

	int y = (pr.first - height) / 2;
	int x = (pr.second - width) / 2;

	auto opts = tuicpp::SelectionWindow::Option {
		.centered = true,
		.multi = false
	};

	auto opt_list = tuicpp::SelectionWindow::OptionList {
		"Option A",
		"Option B",
		"Option C",
		"Option D"
	};

	auto win = new tuicpp::SelectionWindow(
		"Selection Window",
		tuicpp::ScreenInfo {
			.height = height,
			.width = width,
			.y = y,
			.x = x
		},
		opt_list,
		opts
	);

	auto selection = tuicpp::SelectionWindow::Selection {};

	bool yielded = win->yield(selection);
	delete win;

	mvprintw(y, x, "Option selected? %s", yielded ? "yes" : "no");
	for (int i : selection)
		mvprintw(++y, x, "Option \"%s\" selected", opt_list[i].c_str());
	getch();
}

void multi_selection_window()
{
	static int height = 11;
	static int width = 34;

	auto pr = tuicpp::Window::limits();

	int y = (pr.first - height) / 2;
	int x = (pr.second - width) / 2;

	auto opts = tuicpp::SelectionWindow::Option {
		.centered = true,
		.multi = true
	};

	auto opt_list = tuicpp::SelectionWindow::OptionList {
		"Option A",
		"Option B",
		"Option C",
		"Option D"
	};

	auto win = new tuicpp::SelectionWindow(
		"(Multi) Selection Window",
		tuicpp::ScreenInfo {
			.height = height,
			.width = width,
			.y = y,
			.x = x
		},
		opt_list,
		opts
	);

	auto selection = tuicpp::SelectionWindow::Selection {};

	bool yielded = win->yield(selection);
	delete win;

	mvprintw(y, x, "Option selected? %s", yielded ? "yes" : "no");
	for (int i : selection)
		mvprintw(++y, x, "Option \"%s\" selected", opt_list[i].c_str());
	getch();
}
