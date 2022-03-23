#include "global.hpp"

void boxed_window()
{
	static int height = 10;
	static int width = 20;

	auto pr = tuicpp::Window::limits();

	int y = (pr.first - height) / 2;
	int x = (pr.second - width) / 2;

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
}
