#include "global.hpp"

void plain_window()
{
	static int height = 20;
	static int width = 20;

	auto pr = tuicpp::Window::limits();

	int y = (pr.first - height) / 2;
	int x = (pr.second - width) / 2;

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
}
