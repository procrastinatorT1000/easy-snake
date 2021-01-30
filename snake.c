#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <termios.h>
#include <unistd.h>



#define X_COLS   20     /* X coordinate size */
#define Y_STRS   15     /* Y coordinate size */
#define TBL_SZ (X_COLS * Y_STRS)

#define STDIN_FILENO 0
#define NB_DISABLE 0
#define NB_ENABLE 1

#define msleep(x) usleep(x*1000) /* sleep in microseconds */

/*  coord_x[TBL_SZ], coord_y[TBL_SZ] - arrays contains coordinates of
    snakes elements. head of snake located in coord_x[1], coord_y[1]
    dir_x, dir_y is direction of snake moving
    dir_x =  1, dir_y = 0 moves to RIGHT
    dir_x = -1, dir_y = 0 moves to LEFT
    dir_x =  0, dir_y = 1 moves to DOWN
    dir_x =  0, dir_y =-1 moves to UP */
int coord_x[TBL_SZ], coord_y[TBL_SZ];
int dir_x, dir_y;

//snake_len - number of snake elements
//tbl[TBL_SZ][TBL_SZ] - game state mapping table
int snake_len = 2;
char tbl[TBL_SZ][TBL_SZ] = {0};

/*  Used to read key just when it pressed
    to not block main routine */
int kbhit() {
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &fds);
}
/*  Configurate system to get key without
    blocking, do not wait Enter to get key */
void nonblock(int state) {
  struct termios ttystate;

  // Get the terminal state.
  tcgetattr(STDIN_FILENO, &ttystate);

  if (state == NB_ENABLE) {
    // Turn off canonical mode.
    ttystate.c_lflag &= ~ICANON;
    // Minimum of number input read.
    ttystate.c_cc[VMIN] = 1;
  } else if (state == NB_DISABLE) {
    // Turn on canonical mode.
    ttystate.c_lflag |= ICANON;
  }
  // Set the terminal attributes.
  tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

void init_game()
{
    memset( tbl, 0, sizeof(tbl) );
    memset( coord_x, 0, sizeof(coord_x) );
    memset( coord_y, 0, sizeof(coord_y) );

    snake_len = 3;
    coord_x[1] = 4; coord_x[2] = 3; /* Initial position of snake */
    coord_y[1] = 2; coord_y[2] = 2;
    coord_x[3] = 2; coord_y[3] = 2;

    dir_x = 1;
    dir_y = 0;

    nonblock(NB_ENABLE);
}

void deinit_game()
{
    nonblock(NB_DISABLE);
}

/*  */
void move_snake()
{
    for(int i = snake_len; i >= 2; --i)
    {
        coord_x[i] = coord_x[i-1];
        coord_y[i] = coord_y[i-1];
    }

    coord_x[1] += dir_x;
    coord_y[1] += dir_y;

    if( coord_x[1] < 1 )
        coord_x[1] = X_COLS-1;

    if( coord_x[1] > X_COLS-1 )
        coord_x[1] = 1;

    if( coord_y[1] < 1 )
        coord_y[1] = Y_STRS-1;

    if( coord_y[1] > Y_STRS-1 )
        coord_y[1] = 1;


}

void change_direction()
{
    int symbol = getchar(); /* get players action */

    switch(symbol)
    {
        case 'w':
            if(dir_y != 1)
            {
                dir_x = 0; dir_y = -1;
            }
            break;
        case 'a':
            if(dir_x != 1)
            {
                dir_x = -1; dir_y = 0;
            }
            break;
        case 's':
            if(dir_y != -1)
            {
                dir_x = 0; dir_y = 1;
            }
            break;
        case 'd':
            if(dir_x != -1)
            {
                dir_x = 1; dir_y = 0;
            }
            break;
        case 'q':
            deinit_game();
            exit(0);
            break;
        
    }
}

void fill_table()
{
    char blank = ' ';
    char head = '>';
    char snake = '@';
    memset(tbl, blank, sizeof(tbl));

    if(dir_x == 1)  /* head RIGHT */
    {
        head = '>';
    }
    else if(dir_x == -1)  /* head LEFT */
    {
        head = '<';
    }
    else if(dir_y == 1)  /* head DOWN */
    {
        head = 'v';
    }
    else if(dir_y == -1)  /* head UP */
    {
        head = '^';
    }

    tbl[coord_x[1]][coord_y[1]] = head;

    for(int i = 2; i <= snake_len; i++)
    {
        tbl[coord_x[i]][coord_y[i]] = snake;
    }
}

void print_table()
{
    system("clear");
    for( int y=0; y <= Y_STRS; y++ )
    {
        for( int x=0; x <= X_COLS; x++ )
        {
            if( y == 0 || y == Y_STRS || x == 0 || x == X_COLS )
                printf("#");
            else
                printf( "%c", tbl[x][y] );

        }
        printf("\n");
    }
}


int main()
{
    init_game();

    int count = 0;

    while(1)
    {
        fill_table();

        print_table();

        /* handle key if it was pressed */
        if( kbhit() )
            change_direction();

        printf( "c: %d, x: %d, y: %d, len: %d\n", count++, dir_x, dir_y, snake_len );

        move_snake();

        msleep(200);
    }

    return 0;
}