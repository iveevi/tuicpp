#include "global.hpp"

void table_window()
{
	static int height = 20;
	static int width = 50;

	auto pr = tuicpp::Window::limits();

	int y = (pr.first - height) / 2;
	int x = (pr.second - width) / 2;

	auto to_str = [](const float &i, size_t column) {
		if (column == 0)
			return std::to_string(i);
		else
			return std::to_string(i * i);
	};

	auto from = tuicpp::Table <float> ::From({"x", "f(x)"}, to_str);
	from.data = {
		0, 0.5, 1.0, 1.5, 2.0, 2.5,
		3.0, 3.5, 4.0, 4.5, 5.0
	};

	auto win = tuicpp::Table <float> (
		from,
		tuicpp::ScreenInfo {
			.height = height,
			.width = width,
			.y = y,
			.x = x
		}
	);

	win.getc();
}
