#include "global.hpp"

void decorated_window()
{
	static int height = 13;
	static int width = 34;

	auto pr = tuicpp::Window::limits();

	int y = (pr.first - height) / 2;
	int x = (pr.second - width) / 2;

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
}
