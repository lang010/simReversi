//
// SimReversi - a simple game engine for Reversi
//
// Author : Lang Lee <liliang010@gmail.com>
// License: BSD-style
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NONE 0
#define BLACK 1
#define WHITE 2
#define EDGE 3

#define WIDTH 10

int g_depth = 8;
int g_color = 0;

int g_board[WIDTH*WIDTH];
int g_moves[WIDTH*WIDTH];

int board_init();
int board_undo();
int make_move(int m);
int print_board(int m);
int check_win();
int get_move_list(int* moves, int color);
int evaluate(int c);
int alpha_beta_search(int a, int b, int depth, int color, int* ptrMove);

int board_init() {

  g_color = BLACK;
  g_moves[0] = 0;
  memset(g_board, NONE, sizeof(g_board));

  for (int i = 0; i < WIDTH ; i++) {
    g_board[i]
      = g_board[WIDTH*i]
      = g_board[WIDTH*i + WIDTH - 1]
      = g_board[WIDTH*(WIDTH-1) + i]
      = EDGE;
  }

  g_board[WIDTH*(WIDTH/2-1)+WIDTH/2]
    = g_board[WIDTH*(WIDTH/2)+WIDTH/2-1]
    = BLACK;

  g_board[WIDTH*(WIDTH/2-1)+WIDTH/2-1]
    = g_board[WIDTH*(WIDTH/2)+WIDTH/2]
    = WHITE;

  return 0;
}

int board_undo() {
  int n = g_moves[0];
  if (n < 1)
    return n;

  board_init();
  g_moves[0] = n - 1;
  for (int i = 1 ; i < n ; i++)
  {
    make_move(g_moves[i]);
  }
  g_color = g_moves[n]%EDGE;

  return g_moves[0];
}

void move_add(int x) {
  g_moves[0]++;
  g_moves[g_moves[0]] = x;
}

int check_win() {
  int moves[WIDTH*WIDTH];
  int color;
  int n ;
  n = get_move_list(moves, BLACK);
  if (n > 0)
    return 0;
  n = get_move_list(moves, WHITE);
  if (n > 0)
    return 0;

  n = evaluate(BLACK);
  if (n > 0)
  {
    color = BLACK;
    printf("Black win %d points.\n", n);
  } 
  else if ( n < 0)
  {
    color = WHITE;
    printf("White win %d points.\n", -n);
  }else {
    printf("Drawn!\n");
    color = 0;
  }


  return color;
}


int print_board(int m) {
  int n = 0;
  int p;
  int points[EDGE+1];

  memset(points, 0 ,sizeof(points));
  m = m/3;
  m = m % (WIDTH*WIDTH);
  for (int i = 0; i < WIDTH ; i++)
    printf("--");
  printf("\n");
  printf("  ");
  for (int i = 1; i < WIDTH - 1; i++)
    printf("%2d", i);
  printf("\n");
  for (int i = 1; i < WIDTH - 1; i++) {
    printf("%2d", i);
    for (int j = 1 ; j < WIDTH -1 ; j++)
    {
      p = i*WIDTH + j;
    points[g_board[p]]++;
      if (p == m && g_board[p] != NONE) {
      printf(" X");
      continue;
      }
      if (g_board[p] == NONE) {
        printf(" .");
      }else if (g_board[p] == BLACK) {
        printf(" *");
      }else if (g_board[p] == WHITE) {
        printf(" O");
      }else {
        n++;
        printf(" %d", g_board[p]);
      }
    }
    printf("%2d\n",i);
  }

  printf("  ");
  for (int i = 1; i < WIDTH - 1; i++)
    printf("%2d", i);
  printf("\n Black %d - White %d\n", points[BLACK], points[WHITE]);

  for (int i = 0; i < WIDTH ; i++)
    printf("--");
  printf("\n");

  check_win();

  return n;
}

int c;
int self;
int pos;
int p;
int n;

void make_move_unit(int d) {

  p = pos + d;
  while (c == g_board[p]) {
    p += d;
  }
  if (g_board[p] == self) {
    p -= d;
    while (p != pos) {
      g_board[p] = self;
      p -= d;
      n++;
    }
  }
}

void count_unit(int d) {
  p = pos + d;
  while (c == g_board[p]) {
    p += d;
  }
  if (g_board[p] == self) {
    n += (p - pos)/d - 1;
  }
}

int make_move(int m) {

  self = m % EDGE;
  c = self ^ 3;
  n = 0;

  pos = m / EDGE % (WIDTH*WIDTH);
  if ( pos < 1 || pos >= WIDTH*WIDTH
       || g_board[pos] != NONE)
    return 0;

  make_move_unit(1);
  make_move_unit(-1);

  make_move_unit(WIDTH);
  make_move_unit(-WIDTH);


  make_move_unit(1+WIDTH);
  make_move_unit(-1-WIDTH);

  make_move_unit(1-WIDTH);
  make_move_unit(-1+WIDTH);

  if (n > 0)
    g_board[pos] = self;
  
  return n;
}

int count_pos(int cur, int color) {

  n = 0;
  self = color;
  c = self ^ EDGE;
  pos = cur;

  count_unit(1);
  count_unit(-1);

  count_unit(WIDTH);
  count_unit(-WIDTH);

  count_unit(1 + WIDTH);
  count_unit(-1 - WIDTH);

  count_unit(WIDTH - 1);
  count_unit(-WIDTH + 1);

  if (n > 0)
    n = n * (WIDTH*WIDTH*EDGE) + pos * EDGE + self;

  return n;
}

int sort_cmp(const void* a, const void* b) {
  return *(int*)b - *(int*)a;
}

int get_move_list(int* moves, int color) {
  int n = 0;
  int m;

  for (int i = WIDTH + 1 ; i < WIDTH*(WIDTH-1); i++) {
    if (g_board[i] == NONE) {
      m = count_pos(i, color);
      if (m > 0)
        moves[n++] = m;
    }
  }

  qsort( moves, n, sizeof(int), sort_cmp);

  return n;
}

int evaluate(int c) {
  int dc = c ^ EDGE;
  int score = 0;
  int ds = 0;

  for (int i = WIDTH + 1 ; i < WIDTH*(WIDTH-1) ; i++) {
    if (g_board[i] == c) {
      score++;
    }else if (g_board[i] == dc) {
      ds++;
    }
  }

  /*
  if (ds == 0)
    return WIDTH*WIDTH;
  */

  return score - ds;
}

int alpha_beta_search(int a, int b, int depth, int color, int* ptrMove) {
  int moves[WIDTH*WIDTH];

  if (depth == 0) {
    return evaluate(color);
  }

  int val;
  int cur;
  int n = get_move_list(moves, color);
  if (n <= 0) {
    n = get_move_list(moves, color);
  if ( n > 0)
  {
      // For there is no move for current player.
      val = -alpha_beta_search(-b, -a, depth-1, color ^ EDGE, &cur);
    if (val >= b) {
      return val;
    }
    if (val > a ) {
      *ptrMove = 0;
      a = val;
    }
    return a;
  }
  
    return evaluate(color) ;
  }

  int board[WIDTH*WIDTH];
  memcpy(board, g_board, sizeof(board));
  for (int i = 0; i < n ; i++) {
    cur = moves[i];
    make_move(cur);

    val = -alpha_beta_search(-b, -a, depth-1, color ^ EDGE, &cur);

    memcpy(g_board, board, sizeof(board));

    if (val >= b) {
      return val;
    }
    if (val > a ) {
      *ptrMove = moves[i];
      a = val;
    }
  }

  return a;
}

int main() {

  char buf[128];
  int x,y;

  board_init();

  while (scanf("%s", buf) == 1) {
    if (strcmp(buf, "q") == 0) {
      printf("Quit game.\n");
      break;
    }

    if (strcmp(buf, "p") == 0) {
      print_board(0);
  }else if (strcmp(buf, "d") == 0) {
    scanf("%d", &x);
    if (x <= 0)
    {
      printf("Can NOT set depth to %d.\n", x);
      continue;
    }
    g_depth = x;
    printf("Set depth %d.\n", g_depth);
    }else if (strcmp(buf, "w") == 0) {
      g_color = BLACK;
      scanf("%d%d", &x, &y);
      x = ((x*WIDTH)+y)*EDGE + WHITE;
      y = make_move(x);
    if (y <= 0)
      printf("Illegal Move. Skip.\n");
    else {
      print_board(x);
      move_add(x);
    }

    }else if (strcmp(buf, "b") == 0) {
      g_color = WHITE;
      scanf("%d%d", &x, &y);
      x = ((x*WIDTH)+y)*EDGE + BLACK;
    y = make_move(x);
    if (y <= 0)
    printf("Illegal Move. Skip.\n");
    else {
    print_board(x);
    move_add(x);
    }

  }else if (strcmp(buf, "n") == 0) {
    alpha_beta_search( -1000, 1000, g_depth, g_color, &x);
    g_color = g_color ^ EDGE;
    y = make_move(x);
    if (y <= 0)
      printf("No move.\n");
    else {
      print_board(x);
      move_add(x);
    }
  }else if (strcmp(buf, "u") == 0) {
      board_undo();
    print_board(0);
    }else if (strcmp(buf, "s") == 0) {
      board_init();
      print_board(0);
    }else {
      printf("Error Input: %s\n", buf);
      printf("q/p/d/s/n/u/w/b\n\
          q     - quite game\n\
          p     - print board\n\
          d x   - depth x\n\
          s     - start game\n\
          n     - next move\n\
          u     - move undo\n\
          b x y - move black x y\n\
          w x y - move white x y\n");
    }

  }

  return 0;
}
