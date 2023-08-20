#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

// used to play Portal screens :)

#define wikiSpeed 1000

char* readfile(char* filename) {
	FILE *file;
  int fs; // file size
  file = fopen(filename, "r");
  
  if (file == NULL) {
    puts("Error opening file, this is either because the file doesn't exist or you don't have permission to read the requested file.");
    return "oof"; // error opening file
  }

  fseek(file, 0L, SEEK_END);
  fs = ftell(file);
  rewind(file);

  if (fs == -1) {
    puts("That's a directory lol");
    return "oof"; // a directory was provided instead of a file
  }

  char *data = malloc(fs * sizeof(char)); // string with file contents
  fread(data, sizeof(char), fs, file); // read file into string
	
	return data;
}

int main(int argc, char** argv) {
  initscr(); // initialize ncurses
  nodelay(stdscr, TRUE); // don't want delay with getch input
  noecho(); // also, don't echo output
  keypad(stdscr, TRUE); // I also want to use arrow keys for controlling the snake
	curs_set(0); // hides cursor
	
	WINDOW* text;
	WINDOW* wiki;
	WINDOW* photos;

	int w, h; // width and height of window
	getmaxyx(stdscr, h, w);

	text = newwin(h - 2, w - 2, 1, 1); // full screen
	wiki = newwin( (h/2), (w/2), 1, (w/2)-1 ); // first quadrant
	int wikiWidth = (w/2)-2;
	int wikiHeight = (h/2)-2;
	photos = newwin( (h/2), (w/2), (h/2)-1, (w/2)-1 ); // fourth quadrant

	char* lyrics = readfile("1");
	char* wikiData = readfile("wiki");

	int time = 0;
	int sleepTime = 0;
	int speed = wikiSpeed;
	int seek = 0;

	int lX = -1; // lyrics X
	int lY = 0;	// lyrics Y

	while (1) { // repeat until break (which will be end of song)
		if (time % speed == 0 && sleepTime <= 0) {
			int c = lyrics[seek]; // character
			wmove(text, lY + 1, lX + 2);
			waddch(text, ' ');
			if (c == '\n') {
				lX = -1;
				lY++;
				if (lY > h - 5) {
					lY = 0;
					wclear(text);
				}
			} else {
				if (c == '$') { // $leep
												// usage: $5 means sleep for 5000 microseconds
					seek++;
					sleepTime = wikiSpeed * (lyrics[seek] - '0' + 1);
				} else if (c == '~') { // change speed
															 // usage: ~9 means slowest, ~0 means fastest
					seek++;
					speed = wikiSpeed * (lyrics[seek] - '0' + 1);
				} else if (c == '!') { // goodbye...
					endwin();
					exit(0);
				} else {
					lX++;
					wmove(text, lY + 1, lX + 1);
					waddch(text, c);
					wmove(text, lY + 1, lX + 2);
					waddch(text, '_');
				}
			}
			seek++;

		} else if (sleepTime > 0) {
			sleepTime--;
		}	
		if (time % wikiSpeed == 0) {
			int cX = (time / wikiSpeed) % wikiWidth;
			int cY = (time / wikiSpeed) / wikiWidth;
			cY %= wikiHeight;
			wmove(wiki, cY + 1, cX + 1);
			waddch(wiki, wikiData[time / wikiSpeed]);
			cX++;
			cY += cX >= wikiWidth; // bounding logic
			cX %= wikiWidth;
			cY %= wikiHeight;
			if (cX == 0 && cY == 0) {
				wclear(wiki);
			}
			wmove(wiki, cY + 1, cX + 1);
			waddch(wiki, '_'); // fun little cursor :3

			wborder(wiki, '|', '|', '-', '-', '+', '+', '+', '+'); // ascii borders
			wrefresh(text);
			wborder(text, '|', '|', '-', '-', '+', '+', '+', '+'); // ascii borders
			wrefresh(wiki);
		}
		usleep(1);
		time++;
	}
}
