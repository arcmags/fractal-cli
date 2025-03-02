#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

// TODO: color cycle key binding?
// TODO: other fractals?

int main(int argc, char *argv[]) {
    if (argc > 1 &&
    (strcmp(argv[1], "-H") == 0 || strcmp(argv[1], "--help") == 0)) {
        printf("Usage:\n\
  fractal-cli\n\n\
View and navigate the Mandelbrot set in glorious six color ascii.\n\n\
Key Bindings:\n\
  h, j, k, l    move left, down, up, right\n\
  H, J, K, L    move left, down, up, right 8 cells\n\
  n, p          increase, decrease max iterations\n\
  N, P          increase, decrease max iterations by 8\n\
  i, o          zoom in, zoom out\n\
  0 R           reset\n\
  q Q           quit\n");
        exit(0);
    }

    double startX = -2;
    double startY = .75;
    double yToX = 1.66;
    int maxIterations = 256;
    char output[5] = {'@', '8', '$', '#', '&' };
    char charX;
    int ch, yI, xI, max;
    double x, y, xDelta, yDelta, xO, yO, xN, yN;
    double scale = 3;

    initscr();
    start_color();
    if (COLORS <= 8) {
        init_pair(1, 1, 0);
        init_pair(2, 3, 0);
        init_pair(3, 2, 0);
        init_pair(4, 4, 0);
        init_pair(5, 6, 0);
        init_pair(6, 5, 0);
        init_pair(7, 4, 4);
        init_pair(8, 0, 4);
        init_pair(9, 2, 4);
        init_pair(10, 3, 4);
    }
    else {
        init_pair(1, 9, 0);
        init_pair(2, 11, 0);
        init_pair(3, 10, 0);
        init_pair(4, 12, 0);
        init_pair(5, 14, 0);
        init_pair(6, 13, 0);
        init_pair(7, 12, 4);
        init_pair(8, 8, 4);
        init_pair(9, 10, 4);
        init_pair(10, 3, 4);
    }
    attron(A_BOLD);
    curs_set(0);
    keypad(stdscr, TRUE);
    nonl();
    noecho();

    for (;;) {
        clear();
        xDelta = scale / (double)COLS;
        yDelta = xDelta * yToX;

        for (yI=0; yI<LINES; yI++) {
            for (xI=0; xI<COLS; xI++) {
                x = startX + (double)xI * xDelta;
                y = startY - (double)yI * yDelta;
                xO = x;
                yO = y;

                // iterate z^2 + zO while |z^2 + zO| <= 2:
                max = 0;
                while (x*x+y*y <= 4 && max < maxIterations) {
                    xN = x * x - y * y + xO;
                    yN = 2 * x * y + yO;
                    x = xN;
                    y = yN;
                    max++;
                }

                // set char and color from number of iterations required to get |z^2 + zO| > 2:
                if (max < maxIterations) {
                    charX = output[max%5];
                    if (max < 3) {
                        charX = '.';
                    } else if (max == 3) {
                        charX = ',';
                    }
                    // print char:
                    attron(COLOR_PAIR((max/5)%6+1));
                    mvaddch(yI, xI, charX);
                }
            }
        }

        attron(COLOR_PAIR(7));
        for (xI=0; xI<COLS; xI++) {
            mvprintw(LINES-1, xI, " ");
        }
        attron(COLOR_PAIR(9));
        mvprintw(LINES-1, 1, "Mandelbrot");
        if (COLS > 34) {
            attron(COLOR_PAIR(10));
            mvprintw(LINES-1, COLS-23, "%.14f", scale);
            attron(COLOR_PAIR(8));
            printw(" | ");
            attron(COLOR_PAIR(7));
            printw("%d", maxIterations);
        }
        refresh();

        ch = getch();
        switch (ch) {
            case 'h':
            case KEY_LEFT:
                startX = startX - scale / (double)COLS;
                break;
            case 'j':
            case KEY_DOWN:
                startY = startY - scale / (double)COLS * yToX;
                break;
            case 'k':
            case KEY_UP:
                startY = startY + scale / (double)COLS * yToX;
                break;
            case 'l':
            case KEY_RIGHT:
                startX = startX + scale / (double)COLS;
                break;
            case 'H':
                startX = startX - scale / (double)COLS * 8;
                break;
            case 'J':
                startY = startY - scale / (double)COLS * yToX * 8;
                break;
            case 'K':
                startY = startY + scale / (double)COLS * yToX * 8;
                break;
            case 'L':
                startX = startX + scale / (double)COLS * 8;
                break;
            case 'n':
                maxIterations = maxIterations + 1;
                break;
            case 'p':
                maxIterations = maxIterations - 1;
                if (maxIterations < 1) {
                    maxIterations = 1;
                }
                break;
            case 'N':
                maxIterations = maxIterations + 8;
                break;
            case 'P':
                maxIterations = maxIterations - 8;
                if (maxIterations < 1) {
                    maxIterations = 1;
                }
                break;
            case 'i':
            case '+':
                scale = scale * 0.95;
                startX = startX + scale * 0.025;
                startY = startY - scale * 0.015060;
                break;
            case 'o':
            case '-':
                scale = scale * 1.052632;
                startX = startX - scale * 0.0263158;
                startY = startY + scale * 0.0158529;
                break;
            case '0':
            case 'R':
                startX = -2;
                startY = .75;
                scale = 3;
                maxIterations = 256;
                break;
            case 'Q':
            case 'q':
                endwin();
                return 0;
                break;
            case KEY_RESIZE:
                endwin();
                refresh();
                break;
        }
    }
    endwin();
    return 0;
}
