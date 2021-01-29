#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define X_COLS   20     /* X coordinate size */
#define Y_STRS   15     /* Y coordinate size */
#define TBL_SZ (X_COLS * Y_STRS)

/*  coord_x[TBL_SZ], coord_y[TBL_SZ] - arrays contains coordinates of
    snake elements. head of snake located in coord_x[1], coord_y[1] */
int coord_x[TBL_SZ], coord_y[TBL_SZ];

//snake_len - number of snake elements
//tbl[TBL_SZ][TBL_SZ] - game state mapping table
int snale_len = 2;
char tbl[TBL_SZ][TBL_SZ] = {0};

void init_game()
{
    memset( tbl, 0, sizeof(tbl) );
    memset( coord_x, 0, sizeof(coord_x) );
    memset( coord_y, 0, sizeof(coord_y) );

    snale_len = 2;
    coord_x[1] = 3; coord_x[2] = 2; /* Initial position of snake */
    coord_y[1] = 2; coord_y[2] = 2;
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
    for( int i=0; i < Y_STRS; i++ )
    {
        for( int j=0; j < X_COLS; j++ )
        {
            if( i == 0 || i == Y_STRS-1 || j == 0 || j == X_COLS-1 )
                printf("#");
            else
                printf( "%c", tbl[j][i] );

        }
        printf("\n");
    }
}


int main()
{
    init_game();

    fill_table();

    print_table();


    return 0;
}