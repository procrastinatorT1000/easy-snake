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
int snale_len = 2;
char tbl[TBL_SZ][TBL_SZ] = {0};


/*  Configurate system to get key without
    blocking, not wait */
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

    snale_len = 2;
    coord_x[1] = 3; coord_x[2] = 2; /* Initial position of snake */
    coord_y[1] = 2; coord_y[2] = 2;

    dir_x = 1;
    dir_y = 0;

    nonblock(NB_ENABLE);
}

void deinit_game()
{
    nonblock(NB_DISABLE);
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
            nonblock(NB_DISABLE);
            exit(0);
            break;
        
    }
}

void fill_table()
{
    char blank = ' ';
    char snake = '@';
    memset(tbl, blank, sizeof(tbl));

    for(int i = 1; i <= snale_len; i++)
    {
        for(int j = 1; j <= snale_len; j++)
            tbl[coord_x[i]][coord_y[j]] = snake;
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

    while(1)
    {
        fill_table();

        print_table();

        change_direction();
    }

    return 0;
}