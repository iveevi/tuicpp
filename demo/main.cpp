#include "global.hpp"

std::map <std::string, void (*)()> functions {
	{"plain", plain_window},
	{"boxed", boxed_window},
	{"decor", decorated_window},
	{"table", table_window},
	{"editor", editor_window}
};

int main()
{
	// Prompt user for window type
	std::cout << "What type of window would you like to create?\n";
	for (auto const &[key, value] : functions)
		std::cout << "\t" << key << "\n";
	std::cout << "\n> ";

	// Get user input
	std::string input;
	std::cin >> input;

	// Check if input is valid
	if (functions.find(input) == functions.end()) {
		std::cout << "Not a valid window type.\n";
		return 1;
	}

	// Run window type demo
	initscr();
	functions[input]();
	endwin();

	return 0;
}
