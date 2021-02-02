/*
  easy-snake
  Copyright (C) 2021 Slobodchikov Roman

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <termios.h> /* nonblock() */
#include <unistd.h>
#include <time.h>



#define X_COLS   20     /* X coordinate size */
#define Y_STRS   15     /* Y coordinate size */
#define TBL_SZ (X_COLS * Y_STRS)

#define STDIN_FILENO 0
#define NB_DISABLE 0
#define NB_ENABLE 1

#define MAX_DELAY_MS    200       /* Delay for easy level */
#define MIN_DELAY_MS    50        /* Delay for hard level */
#define SPEED_STEP      10        /* How many delay changes when snake eat food */
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
/*  food coordinates */
int food_x, food_y;

//snake_len - number of snake elements
//tbl[TBL_SZ][TBL_SZ] - game state mapping table
int snake_len = 2;
int score     = 0;
char tbl[TBL_SZ][TBL_SZ] = {0};

/* Snake step time */
int delay_ms = 200;
int speed    = 0;
int is_speed_constant = 0;  /* if false, then increment speed before HARD lvl */

/* Speed up snake, faster, higher, stronger */
void speed_up()
{
    if(!is_speed_constant && delay_ms > MIN_DELAY_MS)
    {
        speed++;
        delay_ms = MAX_DELAY_MS - speed*SPEED_STEP;
    }
}

/*  Used to read key just when it pressed
    to not block main routine while key isn't pressed */
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

void init_game( int mode )
{
    memset( tbl, 0, sizeof(tbl) );
    memset( coord_x, 0, sizeof(coord_x) );
    memset( coord_y, 0, sizeof(coord_y) );

    switch( mode )
    {
        case 1:
            delay_ms = MAX_DELAY_MS;
            is_speed_constant = 1;
            speed = 0;
            break;
        case 2:
            delay_ms = MAX_DELAY_MS;
            is_speed_constant = 0;
            speed = 0;
            break;
        case 3:
            delay_ms = MIN_DELAY_MS;
            is_speed_constant = 1;
            speed = 0;
            break;
        default:
            printf("Error: Invalid mode\n");
            exit(1);
            break;
    }

    snake_len = 2;
    score = 0;
    coord_x[1] = 4; coord_x[2] = 3; /* Initial position of snake */
    coord_y[1] = 2; coord_y[2] = 2;

    dir_x = 1;
    dir_y = 0;

    food_x = -1;
    food_y = -1;

    /* config rand() to be more random */
    srand((unsigned)time(NULL));

    nonblock(NB_ENABLE);
}

void deinit_game()
{
    nonblock(NB_DISABLE);
}

void print_score()
{
    printf( "SCORE: %d\n", score );
}

void game_over()
{
    deinit_game();
    print_score();
    printf( "<<<YOU LOOSE!>>>\n" );
    exit(0);
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

    /* check that the snake not eats itself */
    for( int i = 2; i <=snake_len; i++ )
    {
        if( coord_x[1] == coord_x[i] &&
            coord_y[1] == coord_y[i] )
        {
            game_over();
        }
    }
}

void try_to_eat()
{
    /* if head == food */
    if( coord_x[1] == food_x &&
        coord_y[1] == food_y )
    {
        food_x = -1; food_y = -1; /* food was eaten */
        snake_len++;
        score++;
        speed_up();
    }
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

int get_rand_X()
{
    int crd = rand() % X_COLS;
        if( crd == 0 )
            crd = 1;
    return crd;
}

int get_rand_Y()
{
    int crd = rand() % Y_STRS;
        if( crd == 0 )
            crd = 1;
    return crd;
}

void place_food()
{
    /* if food eaten, place new */
    if( food_x == -1 || food_y == -1 )
    {
        food_x = get_rand_X();
        food_y = get_rand_Y();

        /* check that food isn't on snake */
        for( int i = 1, c = 0; i <=snake_len; i++, c++ )
        {
            if( food_x == coord_x[i] &&
                food_y == coord_y[i] )
            {
                food_x = get_rand_X();
                food_y = get_rand_Y();
                i = 1; /* check coordinates from snake head */

                /* protection from infinite loop */
                if( c >= TBL_SZ )
                {
                    printf( "That's all, folks ¯\\_(ツ)_/¯\n");
                    exit(1);
                }
            }
        }
    }
}

void fill_table()
{
    char blank = ' ';
    char head = '>';
    char food = '*';
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

    for(int i = 2; i <= snake_len; i++)
    {
        tbl[coord_x[i]][coord_y[i]] = snake;
    }

    tbl[food_x][food_y] = food;

    tbl[coord_x[1]][coord_y[1]] = head;
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

int menu()
{
    int mode = 0;

    printf( "Use WASD to play\n\n"
            "Enter play mode:\n"
            "1 - Easy\n"
            "2 - Middle\n"
            "3 - Hard\n"
            "q - exit game\n" );

    while(!mode)
    {
        char key = getchar();
        getchar(); /* clear buffer */

        switch(key)
        {
            case '1':
                mode = 1;
                break;
            case '2':
                mode = 2;
                break;
            case '3':
                mode = 3;
                break;
            case 'q':
                printf("Bye.\n");
                exit(0);
                break;
            default:
                printf( "Try again\n" );
                break;
        }
    }

    return mode;
}

int main()
{
    int mode = menu();

    init_game( mode );

    while(1)
    {
        place_food();

        fill_table();

        print_table();

        /* handle key if it was pressed */
        if( kbhit() )
            change_direction();

        move_snake();

        try_to_eat();

        print_score();

        msleep( delay_ms );
    }

    return 0;
}