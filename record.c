#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>

// used to record Portal screens :)

int main(int argc, char** argv) {
  initscr(); // initialize ncurses
  nodelay(stdscr, TRUE); // don't want delay with getch input
  noecho(); // also, don't echo output
  keypad(stdscr, TRUE); // I also want to use arrow keys for controlling the snake
	
	FILE* output;
	if (argc < 2) output = fopen("recording.txt", "w");
	else output = fopen(argv[1], "w");

	
	int key = getch();
	int time = 0;
	while (key != 4) { // 4 is Ctrl-D
		key = getch();
		time++;
		if (key != -1) {
			fprintf(output, "%d\n", time);
			time = 0;
		}
		usleep(1);
	}
	
	fclose(output);
}
