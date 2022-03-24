#include "global.hpp"

void editor_window()
{
	static int height = 10;
	static int width = 50;

	auto pr = tuicpp::Window::limits();

	int y = (pr.first - height) / 2;
	int x = (pr.second - width) / 2;

	std::string name = "Bob Joe";
	std::string email;

	auto win = new tuicpp::FieldEditor(
		"Employee Editor",
		{"Name", "Email"},
		tuicpp::ScreenInfo {
			.height = height,
			.width = width,
			.y = y,
			.x = x
		}
	);

	win->yield({
		tuicpp::yielder(&name),
		tuicpp::yielder(&email)
	});

	delete win;

	mvprintw(y, x, "Name: %s", name.c_str());
	mvprintw(y + 1, x, "Email: %s", email.c_str());
	mvprintw(y + 2, x, "Press any key to quit...");
	getch();
}
