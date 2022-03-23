#include "global.hpp"

int main()
{
	// Iniitialize ncurses
	initscr();

	plain_window();
	
	// End ncurses
	endwin();
}
