#include <stdlib.h>
#include <ncurses.h>
#include <signal.h>

typedef struct graph {
    double startX;      // upper left x-coordinate
    double startY;      // upper left y-coordinate
    double scale;       // width of window
    double yToX;        // ratio of row height to column width
    int maxIterations;  // max number of interations to perform
    char output[5];     // output characters
} graph;

void waitKey(graph*);
void drawScreen(graph*);
void makeScreen(graph*);

int main (void) {
    graph mSet;
    mSet.maxIterations = 40;
    mSet.output[0]='*';
    mSet.output[1]='$';
    mSet.output[2]='@';
    mSet.output[3]='#';
    mSet.output[4]='%';
    mSet.yToX = 1.66;
    mSet.startX = -2;
    mSet.startY = .75;
    mSet.scale = 3;
    makeScreen(&mSet);
    waitKey(&mSet);
    endwin();
    return 0;
}

void waitKey(graph* mSet) {
    int ch;
    for (;;) {
        ch = getch();
        // screen resize:
        if (ch == KEY_RESIZE) {
            endwin();
            refresh();
            drawScreen(mSet);
        }
        // navigation:
        if (ch == 'h') {
            mSet->startX = mSet->startX - mSet->scale / (double)COLS;
            drawScreen(mSet);
        }
        if (ch == 'H') {
            mSet->startX = mSet->startX - (mSet->scale / (double)COLS)*10;
            drawScreen(mSet);
        }
        if (ch == 'j') {
            mSet->startY = mSet->startY - (mSet->scale / (double)COLS) * mSet->yToX;
            drawScreen(mSet);
        }
        if (ch == 'J') {
            mSet->startY = mSet->startY - (mSet->scale / (double)COLS) * mSet->yToX*10;
            drawScreen(mSet);
        }
        if (ch == 'l') {
            mSet->startX = mSet->startX + mSet->scale / (double)COLS;
            drawScreen(mSet);
        }
        if (ch == 'L') {
            mSet->startX = mSet->startX + (mSet->scale / (double)COLS) *10;
            drawScreen(mSet);
        }
        if (ch == 'k') {
            mSet->startY = mSet->startY + (mSet->scale / (double)COLS) * mSet->yToX;
            drawScreen(mSet);
        }
        if (ch == 'K') {
            mSet->startY = mSet->startY + (mSet->scale / (double)COLS) * mSet->yToX*10;
            drawScreen(mSet);
        }
        // iterations::
        if (ch == 'u') {
            mSet->maxIterations=mSet->maxIterations+1;
            drawScreen(mSet);
        }
        if (ch == 'y') {
            mSet->maxIterations=mSet->maxIterations-1;
            drawScreen(mSet);
        }
        // zoom:
        if (ch == 'n') {
            mSet->startX = mSet->startX + mSet->scale*0.025;
            mSet->startY = mSet->startY - mSet->scale*0.015060;
            mSet->scale = mSet->scale*0.95;
            mSet->maxIterations= mSet->maxIterations + 1;
            drawScreen(mSet);
        }
        if (ch == 'p') {
            mSet->startX = mSet->startX - mSet->scale*0.0263158;
            mSet->startY = mSet->startY + mSet->scale*0.0158529;
            mSet->scale = mSet->scale*1.052632;
            mSet->maxIterations= mSet->maxIterations -1;
            drawScreen(mSet);
        }
        // reset:
        if (ch == '0') {
            mSet->startX = -2;
            mSet->startY = .75;
            mSet->scale = 3;
            mSet->maxIterations=40;
            drawScreen(mSet);
        }
        // quit:
        if (ch == 'q') {
            return;
        }
    }
}

void makeScreen(graph* mSet) {
    initscr();
    if(has_colors() == FALSE) {
        endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}

    curs_set(0);
    keypad(stdscr, TRUE);
    nonl();
    noecho();

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_BLUE, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(8, COLOR_WHITE, COLOR_BLUE);

    attron(A_BOLD);

    drawScreen(mSet);
}


// draws the screen
void drawScreen(graph* mSet) {
    clear();

    double xDelta = mSet->scale / (double)COLS;
    double yDelta = xDelta * mSet->yToX;

    int yI;
    int xI;
    double x;
    double y;
    int max;
    char charX;

    for (yI=0; yI<LINES; yI++) {
        for (xI=0; xI<COLS; xI++) {
            move(yI,xI);

            // get x and y of current grid character
            x = mSet->startX + ((double)xI) * xDelta;
            y = mSet->startY - ((double)yI) * yDelta;
            // set initial xO and yO
            double xO=x;
            double yO=y;
            // temp variables
            double xN;
            double yN;

            // iterate z^2 + zO while |z^2 + zO| <= 2
            max=0;
            while (x*x+y*y <= 4 && max < mSet->maxIterations) {
                xN= x*x - y*y + xO;
                yN = 2*x*y + yO;
                x=xN;
                y=yN;
                max++;
            }

            // set character and color depending on number of iterations required to get |z^2 + zO| > 2
            if (max < mSet->maxIterations) {

                charX=mSet->output[max%5];
                if (max < 3) {
                    charX='.';
                }
                if (max == 3) {
                    charX=',';
                }

                attron(COLOR_PAIR((max/5)%6+2));
                addch(charX);
            }
        }
    }

    // display commands on bottom of window
    attron(COLOR_PAIR(1));
    mvprintw(LINES-1,0,"iterations: ");
    attron(COLOR_PAIR(5));
    mvprintw(LINES-1,12,"%d ",mSet->maxIterations);
    attron(COLOR_PAIR(8));
    mvprintw(LINES-1,(COLS-74),"[hjkl]");
    attron(COLOR_PAIR(1));
    mvprintw(LINES-1,(COLS-68),"/");
    attron(COLOR_PAIR(8));
    mvprintw(LINES-1,(COLS-67),"[HJKL]");
    attron(COLOR_PAIR(1));
    mvprintw(LINES-1,(COLS-61),"navigate   ");
    attron(COLOR_PAIR(8));
    mvprintw(LINES-1,(COLS-50),"[np]");
    attron(COLOR_PAIR(1));
    mvprintw(LINES-1,(COLS-46),"+zoom-   ");
    attron(COLOR_PAIR(8));
    mvprintw(LINES-1,(COLS-37),"[yu]");
    attron(COLOR_PAIR(1));
    mvprintw(LINES-1,(COLS-33),"+iterations-   ");
    attron(COLOR_PAIR(8));
    mvprintw(LINES-1,(COLS-18),"[0]");
    attron(COLOR_PAIR(1));
    mvprintw(LINES-1,(COLS-15),"reset   ");
    attron(COLOR_PAIR(8));
    mvprintw(LINES-1,(COLS-7),"[q]");
    attron(COLOR_PAIR(1));
    mvprintw(LINES-1,(COLS-4),"quit");
    refresh();
}
