#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <windows.h>
#include <time.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 4

// board
int board[DIM_MAX][DIM_MAX];
int copy[DIM_MAX][DIM_MIN]; //for GOD mode

// dimensions
int d;

//to ensure that the numbers in the intitial board configuration are all different
int random[17];

//search tree
typedef struct node
{
    int state[DIM_MAX][DIM_MIN];
    struct node* next;
}node;

node* head;
node* trav;
node* cursor;

// prototypes
void clear(void);
void greet(void);
void init(void);
int check(int, int);
int invert(int);
void parity(void);
void draw(void);
int move(int tile);
int won(void);

//GOD mode special functions
void GOD(void);
void ctrl_c(void);
int find_blank(int*, int*);
int test(int, int);
int heuristic(void);
int occurrence(void);
void change(void);
void find(int a, int* b, int* c);

int main(void)
{
    int moves = 0;
    int tile;
    printf("Enter the dimension of the board::");
    scanf("%d", &d);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    for(;;)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();



        // check for win
        if (won() == 1)
        {
            printf("Congratulations!\n");
            printf("No. of moves used = %d", moves);
            break;
        }
        // prompt for move if not in GOD mode
        printf("Tile to move:(Enter 1213 to activate GOD mode) ");
        scanf("%d", &tile);
        if(tile == 1213)
        {
                GOD();
                break;
        }
        // move if possible, else report illegality
        if (move(tile)!= 1)
        {
            printf("\nIllegal move.\n");
            Sleep(1200);
        }
        else
        {
            moves++;
        }

        // sleep thread for animation's sake
        Sleep(200);
    }

    // success
    return 0;
}

/*
 * Clears screen
 */
void clear(void)
{
    system("cls");
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    Sleep(1000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).
 */
void init(void)
{
    int k = (d * d);
    int i, j, cnt;
    int m;
    cnt = 0;
    for(i = 0; i < d; i++)
    {
        for(j = 0; j < d; j++)
        {
            do
            {
                m = (clock() + rand())  % k;
            }
            while(check(m, cnt) == 0);
            board[i][j] = m;
            random[cnt] = board[i][j];
            cnt++;
        }
    }
    parity();
}


/**
 * Checks if any number has been repeated already and then returns 0 if it has occurred, otherwise 1
 */
int check(int a, int b)
{
    int i, j;
    for(i = 0; i < b; i++)
    {
        if(a == random[i])
            return 0;
    }
    return 1;
}

/**
 * Ensures that the initial board configuration is solvable
 */
void parity(void)
{

    int position, blank, temp, result, inver;
    int i, j;
    inver = 0;
    position = 0;
    for(i = 0; i < d; i++)
    {
        for(j = 0; j < d; j++)
        {
            result = invert(position);
            if(result == -1)
            {
                blank = i + 1;

            }
            else
            {
                inver += result;
            }

            position++;
        }
    }

    if((d % 2) == 0 && ((inver + blank) % 2) != 0)
    {
            if(board[0][0] == 0)
            {
                temp = board[0][1];
                board[0][1] = board[0][2];
                board[0][2] = temp;
                random[1] = board[0][1];
                random[2] = board[0][2];
            }
            else if(board[0][1] == 0)
            {
                temp = board[0][0];
                board[0][0] = board[0][2];
                board[0][2] = temp;
                random[0] = board[0][0];
                random[2] = board[0][2];
            }
            else
            {
                temp = board[0][0];
                board[0][0] = board[0][1];
                board[0][1] = temp;
                random[0] = board[0][0];
                random[1] = board[0][1];
            }
    }
    else if((d % 2) != 0 && (inver % 2) != 0)
    {
            if(board[0][0] == 0)
            {
                temp = board[0][1];
                board[0][1] = board[0][2];
                board[0][2] = temp;
                random[1] = board[0][1];
                random[2] = board[0][2];
            }
            else if(board[0][1] == 0)
            {
                temp = board[0][0];
                board[0][0] = board[0][2];
                board[0][2] = temp;
                random[0] = board[0][0];
                random[2] = board[0][2];
            }
            else
            {
                temp = board[0][0];
                board[0][0] = board[0][1];
                board[0][1] = temp;
                random[0] = board[0][0];
                random[1] = board[0][1];
            }
    }
    position = 0;
    inver = 0;
    blank = 0;
    for(i = 0; i < d; i++)
    {
        for(j = 0; j < d; j++)
        {
            result = invert(position);
            if(result == -1)
            {
                blank = i + 1;
            }
            else
            {
                inver += result;
            }

            position++;
        }
    }

}

/**
 * Checks for solvability of the puzzle
 */
int invert(int k)
{
    int i, counter;
    counter = 0;
    if(random[k] == 0)
        return -1;
    for(i = k + 1; i < d * d; i++)
    {
        if(random[i] != 0)
        {
         if(random[k] > random[i])
                counter++;
        }
    }
    return counter;

}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    int i,j ;

    for(i = 0; i < d; i++)
    {
        for(j = 0; j < d; j++)
        {
            if(board[i][j] != 0)
            {
                printf("%2i  ", board[i][j]);
            }
            else
            {
                printf("__  ");
            }
        }
        printf("\n");
    }
}

/*
 ** This is the GOD mode
 */
 void GOD(void)
 {
     trav = NULL;
     head = (node*)malloc(sizeof(node));
     trav = head;
     int change;
     int i,j, blank_row, blank_column;
     for(;;)
     {
        clear();
        ctrl_c();
        find_blank(&blank_row, &blank_column);
        change = test(blank_row, blank_column);

        move(change);
        draw();
        Sleep(1000);
        if(won() == 1)
        {
            printf("Tathaastu...SOlVED!!!");
            while(head != NULL)
            {
                trav = head;
                head = head -> next;
                free(trav);
            }
            break;
        }
        trav->next = malloc(sizeof(node));
        trav = trav->next;
    }

 }

 /*
  **copies state of the board into state
  */
 void ctrl_c(void)
 {
     int i, j;
     for(i = 0; i < d; i++)
     {
         for(j = 0; j < d; j++)
         {
             trav->state[i][j] = board[i][j];
         }
     }
     trav->next = NULL;
 }

 /*
  ** find the location of the blank space
  */
 int find_blank(int* a, int* b)
 {
     int i, j;
     for(i = 0; i < d; i++)
     {
         for(j = 0; j < d; j++)
         {
             if(board[i][j] == 0)
             {
                 *a = i;
                 *b = j;
                 return 1;
             }
         }
     }
 }

 int test(int x, int y)
 {
     int min, minof;
     int cost[4];
     int costof[4];
     int i, j, temp;
     int index = 0;
    for(i = 0; i < d; i++)
    {
        for(j = 0; j < d; j++)
        {
            copy[i][j] = board[i][j];
        }
    }
    if((x - 1) >= 0)
    {
        temp = copy[x - 1][y];
        copy[x - 1][y] = copy[x][y];
        copy[x][y] = temp;

        cost[index] = heuristic();
        costof[index] = copy[x][y];
        index++;

    }

    //swap back
    temp = copy[x - 1][y];
    copy[x - 1][y] = copy[x][y];
    copy[x][y] = temp;
    if((x + 1) < d)
    {
        temp = copy[x + 1][y];
        copy[x + 1][y] = copy[x][y];
        copy[x][y] = temp;
        cost[index] = heuristic();
        costof[index] = copy[x][y];
        index++;

    }
    temp = copy[x + 1][y];
    copy[x + 1][y] = copy[x][y];
    copy[x][y] = temp;
    if((y - 1) >= 0)
    {
        temp = copy[x][y - 1];
        copy[x][y - 1] = copy[x][y];
        copy[x][y] = temp;

        cost[index] = heuristic();
        costof[index] = copy[x][y];
        index++;

    }
    temp = copy[x][y - 1];
    copy[x][y - 1] = copy[x][y];
    copy[x][y] = temp;
    if((y + 1) < d)
    {
        temp = copy[x][y + 1];
        copy[x][y + 1] = copy[x][y];
        copy[x][y] = temp;

        cost[index] = heuristic();
        costof[index] = copy[x][y];
        index++;

    }
    min = cost[0];
    minof = costof[0];
    for(i = 0; i < index; i++)
    {
        if((cost[i] < min) && (cost[i] != 10000))
        {
            min = cost[i];
            minof = costof[i];
        }
    }

    return minof;

 }

 int heuristic(void)
 {
     int i, j, cnt;
     int place1, place2, x, y;
     cursor = NULL;
     cursor = head;

     while(cursor != NULL)
     {

        cnt = occurrence();
        if(cnt == d*d)
        {
            return 30000;
        }
        cursor = cursor->next;
     }

     int misplaced = 0;
     int place = 1;
     for(i = 0; i < d; i++)
     {
         find(board[i][j], &place1, &place2);
         for(j = 0; j < d; j++)
         {
             if(copy[i][j] != place)
             {
                 if(place1 > i)
                 {
                     x = place1 -1;
                 }
                 else
                 {
                     x = i - place1;
                 }
                 if(place2 > j)
                 {
                     y = place2 - i;
                 }
                 else
                 {
                     y = i - place2;
                 }
                misplaced = misplaced + x + y;
             }
             if((i == d - 1) && (j == d - 2))
             {
                 place = 0;
             }
             else
             {
                 place++;
             }
         }
     }
     return misplaced;
 }

 /*
 ** finds the position of the tile that is supposed to be
 */
 void find(int a, int* b, int* c)
 {
     int i, j;
     int cnt = 0;
     for(i = 0; i < d; i++)
     {
         for(j = 0; j < d; j++)
         {
             if(a == cnt)
             {
                 * b = i;
                 * c = j;
             }
             cnt++;
         }
     }
 }

 /*
  **checks if the board configuration has previously been encountered
  */
  int occurrence(void)
  {
      int cnt = 0;
      int i, j;
      cnt = 0;
      for(i = 0; i < d; i++)
      {
        for(j = 0; j < d; j++)
        {
            if(cursor->state[i][j] == copy[i][j])
                cnt++;
        }
      }
      return cnt;
  }

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false.
 */
int move(int tile)
{
    if (isalpha(tile))
    {
        return 0;
    }
    int zero1 = 0;
    int zero2 = 0;
    int tile1 = 0;
    int tile2 = 0;
    int dummy = 0;
    int i, j;
    for(i = 0; i < d; i++)
    {
        for(j = 0; j < d; j++)
        {
           if(board[i][j] == 0)
           {
               zero1 = i;
               zero2 = j;
           }
           if(board[i][j] == tile)
           {
               tile1 = i;
               tile2 = j;
           }
        }
    }
    printf("Moving tile %d\n", tile);
    //if same row
    if(zero1 == tile1)
    {
        //if adjacent columns
        if (((zero2 - tile2) == 1) || ((tile2 - zero2) == 1))
        {
            dummy = board[zero1][zero2];
            board[zero1][zero2] = board[tile1][tile2];
            board[tile1][tile2] = dummy;
            return 1;
        }
    }
    //if same column
    else if(zero2 == tile2)
    {
        //if adjacent rows
        if (((zero1 - tile1) == 1) || ((tile1 - zero1) == 1))
        {
            dummy = board[zero1][zero2];
            board[zero1][zero2] = board[tile1][tile2];
            board[tile1][tile2] = dummy;
            return 1;
        }
    }
    else
    {
        return 0;
    }
    return 0;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration),
 * else false.
 */
int won(void)
{
    int k = 1;
    int i, j;
    for(i = 0; i < d; i++)
    {
        for(j = 0; j < d; j++)
        {
            if ((i != (d-1)) || (j != (d-1)))
            {
               if(board[i][j] != k)
                {
                    return 0;
                }
                k++;
            }

        }
    }
    return 1;
}
