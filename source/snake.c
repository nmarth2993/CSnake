#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <ncurses.h>

#define MS_DELAY 75
#define MAX_LEN 20
#define MAX_X 119
#define MAX_Y 29
#define STARTX 1
#define STARTY 1
#define APPLE_CHAR 'A'
#define cmppos(a, b) ((a.body[0].x == b.x) && (a.body[0].y == b.y))
#define wallcheck (snake.body[0].x < 0 || snake.body[0].y < 0 || snake.body[0].x > MAX_X || snake.body[0].y > MAX_Y)

enum
{
    NORTH = 0,
    SOUTH,
    EAST,
    WEST
};

typedef struct
{
    char x;
    char y;
} Position;

typedef struct
{
    int direction;
    Position body[MAX_LEN];
} Snake;

Position nullpos = {-1, -1};
struct timespec DELAY = {0, MS_DELAY * 1000000};
struct timespec SECOND_DELAY = {1, 0};

int gameloop(WINDOW *);
WINDOW *newscreen(void);
Snake newgame(void);
Position newapple(void);
int length(Snake);
int grow(Snake *);
Position advance(Snake *, Position);
void drawsnake(WINDOW *, Snake);
void drawapple(WINDOW *, Position);

int main(int argc, char const *argv[])
{

    srand(time(0));
    int result = gameloop(newscreen());
    // clean up
    endwin();

    if (result == -1)
    {
        printf("loss |  ||  ||  | _\n");
    }
    else if (result == 0)
    {
        printf("player died\n");
    }
    else
    {
        printf("\n\\ \\ / /           | |  | (_)     | | \n \\ V /___  _   _  | |  | |_ _ __ | | \n  \\ // _ \\| | | | | |/\\| | | '_ \\| | \n  | | (_) | |_| | \\  /\\  / | | | |_| \n  \\_/\\___/ \\__,_|  \\/  \\/|_|_| |_(_) \n");
    }

    return 0;
}

//inits ncurses settings and returns pointer to screen
WINDOW *newscreen(void)
{
    WINDOW *window = initscr();
    noecho();
    keypad(window, TRUE);
    nodelay(window, TRUE);
    curs_set(0);
    cbreak();
    if (has_colors)
    {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_BLUE, COLOR_BLACK);
        init_pair(4, COLOR_CYAN, COLOR_BLACK);
    }
    return window;
}

//creates new game, inits and returns snake
Snake newgame(void)
{
    Snake snake;
    snake.direction = EAST;
    Position head;
    head.x = STARTX;
    head.y = STARTY;
    snake.body[0] = head;
    for (int i = 1; i < MAX_LEN; i++)
    {
        snake.body[i] = nullpos;
    }
    return snake;
}

//returns the length of the snake's body
int length(Snake snake)
{
    int i = 0;
    while (i < MAX_LEN && snake.body[i].x != -1)
    {
        i++;
    }
    return i;
}

//returns position of apple
Position newapple(void)
{
    Position pos = {(rand() % MAX_X), (rand() % MAX_Y)};
    return pos;
}

//advances all segments by one given a new head position
//returns position removed from queue
Position advance(Snake *snake, Position head)
{
    Position last = snake->body[length(*snake) - 1];
    Position tmp[19];
    for (int i = 0; i < MAX_LEN; i++)
    {
        tmp[i] = snake->body[i];
    }
    for (int i = 0; i < length(*snake) - 1 && snake->body[i + 1].x != -1; i++)
    {
        snake->body[i + 1] = tmp[i];
    }
    snake->body[0] = head;
    return last;
    // for (int i = 0; i < 19; i++)
    // {
    //     wmove(stdscr, i + 1, 80);
    //     wprintw(stdscr, "tmp[%d]: (%d, %d)", i, tmp[i].x, tmp[i].y);
    // }
    // wrefresh(stdscr);
    // nanosleep(&DELAY, 0);
}

//adds one segment to the snake's body
//returns 1 if snake is already at max length (win condition)
int grow(Snake *snake)
{
    if (length(*snake) == MAX_LEN)
    {
        return 1;
    }
    // Position pos;
    snake->body[length(*snake)] = snake->body[length(*snake) - 1];
    return 0;
}

//checks collision of head with body and walls
//returns 0 for no collision
//returns 1 for collision
int checkcollision(Snake snake)
{
    // wmove(stdscr, 2, 100);
    for (int i = 2; i < length(snake) - 1; i++)
    {
        if (cmppos(snake, snake.body[i]))
        {
            return 1;
        }
    }
    return wallcheck;
}

//main game loop
int gameloop(WINDOW *window)
{
    Snake snake = newgame();
    Position apple = newapple();
    Position head;
    Position space;
    head.x = snake.body[0].x;
    head.y = snake.body[0].y;
    wmove(window, 4, 100);
    int result = 0;
    int alive = 1;
    int ch;
    while (alive)
    {
        space = advance(&snake, head);
        if ((ch = getch()) != ERR)
        {
            // wmove(window, 3, 100);
            // wprintw(window, "keycode: %d", ch);
            // wrefresh(window);
            // nanosleep(&SECOND_DELAY, 0);
            switch (ch)
            {
            case KEY_UP:
                if (snake.direction != SOUTH)
                {
                    snake.direction = NORTH;
                }
                break;
            case KEY_DOWN:
                if (snake.direction != NORTH)
                {
                    snake.direction = SOUTH;
                }
                break;
            case KEY_RIGHT:
                if (snake.direction != WEST)
                {
                    snake.direction = EAST;
                }
                break;
            case KEY_LEFT:
                if (snake.direction != EAST)
                {
                    snake.direction = WEST;
                }
                break;
            case 'q':
                endwin();
                printf("q key pressed, exiting...\n");
                exit(0);
                break;
            }
        }
        //could refactor to switch
        if (snake.direction == NORTH)
        {
            snake.body[0].y--;
        }
        else if (snake.direction == SOUTH)
        {
            snake.body[0].y++;
        }
        else if (snake.direction == EAST)
        {
            snake.body[0].x++;
        }
        else if (snake.direction == WEST)
        {
            snake.body[0].x--;
        }
        head.x = snake.body[0].x;
        head.y = snake.body[0].y;
        if (cmppos(snake, apple))
        {
            apple = newapple();
            if (result = grow(&snake))
            {
                alive = 0;
            }
        }
        if (alive && checkcollision(snake))
        {
            result = -1;
            alive = 0;
        }
        wclear(window); //I guess slight flickering will have to do for now
        // wmove(window, space.y, space.x);
        // waddch(window, 32);
        drawsnake(window, snake);
        drawapple(window, apple);
        // wmove(window, 1, 100);
        // wprintw(window, "len: %d", length(snake));
        // wmove(window, 5, 100);
        // wprintw(window, "head: (%d, %d)", snake.body[0].x, snake.body[0].y);
        // wmove(window, 8, 100);
        // wprintw(window, "space: (%d, %d)", space.x, space.y);
        // for (int index = 6; index < 25; index++)
        // {
        //     wmove(window, index, 100);
        //     wprintw(window, "p%d: (%d, %d)", index - 4, snake.body[index - 6].x, snake.body[index - 6].y);
        // }

        // wprintw(window, "dir: %c posx: %d posy: %d",
        //         (snake.direction == NORTH ? 'N' : snake.direction == SOUTH ? 'S' : snake.direction == EAST ? 'E' : 'W'), snake.body[0].x, snake.body[0].y);
        wrefresh(window);
        nanosleep(&DELAY, 0);
    }
    return result;
}

//draws the snake
//H for head
//O for body segment
void drawsnake(WINDOW *window, Snake snake)
{
    for (int i = 0; i < length(snake); i++)
    {
        // wmove(window, 2, 70);
        // wprintw(window, "%d: (%d, %d)", i, snake.body[i].x, snake.body[i].y);
        // wrefresh(window);
        // nanosleep(&SECOND_DELAY, 0);
        wmove(window, snake.body[i].y, snake.body[i].x);
        if (!i)
        {
            char c;
            wattron(window, COLOR_PAIR(2));
            switch (snake.direction)
            {
            case NORTH:
                c = '^';
                break;
            case SOUTH:
                c = 'V';
                break;
            case EAST:
                c = '>';
                break;
            case WEST:
                c = '<';
                break;
            }
            waddch(window, c);
        }
        else
        {
            if (i == 19)
            {
                wattron(window, COLOR_PAIR(4));
            }
            else
            {
                wattron(window, COLOR_PAIR(3));
            }
            waddch(window, i % 3 ? 'O' : 'o');
        }
        // waddch(window, (i ? 'O' : 'H'));
        // waddch(window, 'O');
    }
    // wmove(snake.body[length(snake) - 1].x, snake.body[length(snake) - 1].y);
}

//draws APPLE_CHAR at pos on *window
void drawapple(WINDOW *window, Position pos)
{
    wattron(window, COLOR_PAIR(1));
    // wmove(window, 6, 100);
    // wprintw(window, "apple: (%d, %d)", pos.x, pos.y);
    wmove(window, pos.y, pos.x);
    waddch(window, APPLE_CHAR);
}