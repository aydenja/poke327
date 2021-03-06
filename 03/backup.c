#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>

#include "heap.h"

#define malloc(size) ({          \
  void *_tmp;                    \
  assert((_tmp = malloc(size))); \
  _tmp;                          \
})

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;




typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef uint8_t pair_t[num_dims];

#define MAP_X              80
#define MAP_Y              21
#define MIN_TREES          10
#define MIN_BOULDERS       10
#define TREE_PROB          95
#define BOULDER_PROB       95
#define WORLD_DIM          399
#define WORLD_CEN          199

#define mappair(pair) (m->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (m->map[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])
#define heightxy(x, y) (m->height[y][x])

typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_debug,
  ter_boulder,
  ter_tree,
  ter_path,
  ter_mart,
  ter_center,
  ter_grass,
  ter_clearing,
  ter_mountain,
  ter_forest,
} terrain_type_t;

typedef struct square {
  heap_node_t *hn;
  uint8_t pos[2];
  int32_t cost;
  terrain_type_t t;
} square_t;

typedef struct map {
  terrain_type_t map[MAP_Y][MAP_X];
  uint8_t height[MAP_Y][MAP_X];
  pair_t pc;
  uint8_t n, s, e, w;
} map_t;

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

/*
static int32_t square_cmp(const void *key, const void *with) {
  return ((square_t *) key)->cost - ((square_t *) with)->cost;
}
*/

static int32_t edge_penalty(uint8_t x, uint8_t y)
{
  return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}



int get_hiker_cost(terrain_type_t t){
  switch(t){
    case ter_path:
      return 10;
    case ter_grass:
      return 15;
    case ter_clearing:
      return 10;
    case ter_mountain:
      return 15;
    case ter_forest:
      return 15;
    default:
      return INT_MAX/10;
  }
}

int get_rival_cost(terrain_type_t t){
  switch(t){
    case ter_path:
      return 10;
    case ter_grass:
      return 20;
    case ter_clearing:
      return 10;
    default:
      return INT_MAX/10;
  }
}

static void dijkstra_square(map_t *m, pair_t from, int (*f)(terrain_type_t))
{
  static path_t path[MAP_Y][MAP_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      path[y][x].cost = INT_MAX/10;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

//insert every pos into heap
  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }

  //path_t *t = heap_peek_min(&h);
 // printf("new min is: %d\n", t->pos[dim_x]);

  //printf("Start - Size: %d\n", h.size);
  //while heap is not empty remove smallest thing
  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    //printf("test %d\n", (p->cost + get_hiker_cost(mappair(p->pos))));
    
    //all ifs that follow are calculating cost for each dir, only 4 need 4 more
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         ((p->cost + f(mappair(p->pos))) ))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        ((p->cost + f(mappair(p->pos))) );
      //printf("new cost for(%d, %d): %d\n",p->pos[dim_x] ,p->pos[dim_y] - 1, path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost );
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] -1  ].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] -1 ].cost >
         ((p->cost + f(mappair(p->pos))) ))) {
      path[p->pos[dim_y]][p->pos[dim_x] -1 ].cost =
        ((p->cost + f(mappair(p->pos))) );
      
      //printf("new cost for(%d, %d): %d\n",p->pos[dim_x]-1 ,p->pos[dim_y] , path[p->pos[dim_y]][p->pos[dim_x]  -1  ].cost );
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]]
                                           [p->pos[dim_x] -1 ].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] +1  ].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] +1 ].cost >
         ((p->cost + f(mappair(p->pos))) ))) {
      path[p->pos[dim_y]][p->pos[dim_x] +1 ].cost =
        ((p->cost + f(mappair(p->pos))) );
      
      //printf("new cost for(%d, %d): %d\n",p->pos[dim_x] +1 ,p->pos[dim_y], path[p->pos[dim_y] ][p->pos[dim_x] +1   ].cost );
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]]
                                           [p->pos[dim_x] +1 ].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         ((p->cost + f(mappair(p->pos))) ))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        ((p->cost + f(mappair(p->pos))) );
      
      //printf("new cost for(%d, %d): %d\n",p->pos[dim_x] ,p->pos[dim_y] + 1, path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost );
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]+1  ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]+1  ].cost >
         ((p->cost + f(mappair(p->pos))) ))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] +1 ].cost =
        ((p->cost + f(mappair(p->pos))) );
      
      //printf("new cost for(%d, %d): %d\n",p->pos[dim_x] ,p->pos[dim_y] + 1, path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost );
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x] +1 ].hn);
    }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]+1  ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]+1  ].cost >
         ((p->cost + f(mappair(p->pos))) ))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] +1 ].cost =
        ((p->cost + f(mappair(p->pos))) );
      
      //printf("new cost for(%d, %d): %d\n",p->pos[dim_x] ,p->pos[dim_y] + 1, path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost );
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x] +1 ].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]-1  ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]-1  ].cost >
         ((p->cost + f(mappair(p->pos))) ))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x] -1 ].cost =
        ((p->cost + f(mappair(p->pos))) );
      
      //printf("new cost for(%d, %d): %d\n",p->pos[dim_x] ,p->pos[dim_y] + 1, path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost );
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] +1]
                                           [p->pos[dim_x] -1 ].hn);
    }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]-1  ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]-1  ].cost >
         ((p->cost + f(mappair(p->pos))) ))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x] -1 ].cost =
        ((p->cost + f(mappair(p->pos))) );
      
      //printf("new cost for(%d, %d): %d\n",p->pos[dim_x] ,p->pos[dim_y] + 1, path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost );
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x] -1 ].hn);
    }
    //printf("mid - Size: %d\n", h.size);
    //t = heap_peek_min(&h);
    //printf("new min is: %d\n", t->pos[dim_x]);
  }

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (path[y][x].cost == 214748364){
        printf("   ");
      }
      else{
        printf("%02d ", path[y][x].cost%100);
      }
    }
    printf("\n");
  }

  heap_delete(&h);
  return;

}


static void dijkstra_path(map_t *m, pair_t from, pair_t to)
{
  static path_t path[MAP_Y][MAP_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

//insert every pos into heap
  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }


//while heap is not empty remove smallest thing
  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    //if destination is found, delete for shortest path
    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        mapxy(x, y) = ter_path;
        heightxy(x, y) = 0;
      }
      heap_delete(&h);
      return;
    }

    
    //all ifs that follow are calculating cost for each dir, only 4 need 4 more
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1)))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
        //setting path dir, delete 
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1)))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }
}

static int build_paths(map_t *m)
{
  pair_t from, to;

  from[dim_x] = 1;
  to[dim_x] = MAP_X - 2;
  from[dim_y] = m->w;
  to[dim_y] = m->e;

  dijkstra_path(m, from, to);

  from[dim_y] = 1;
  to[dim_y] = MAP_Y - 2;
  from[dim_x] = m->n;
  to[dim_x] = m->s;

  dijkstra_path(m, from, to);

  return 0;
}

static int gaussian[5][5] = {
  {  1,  4,  7,  4,  1 },
  {  4, 16, 26, 16,  4 },
  {  7, 26, 41, 26,  7 },
  {  4, 16, 26, 16,  4 },
  {  1,  4,  7,  4,  1 }
};

static int smooth_height(map_t *m)
{
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  /*  FILE *out;*/
  uint8_t height[MAP_Y][MAP_X];

  memset(&height, 0, sizeof (height));

  /* Seed with some values */
  for (i = 1; i < 255; i += 20) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (height[y][x]);
    height[y][x] = i;
    if (i == 1) {
      head = tail = malloc(sizeof (*tail));
    } else {
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);
  */
  
  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = height[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1]) {
      height[y - 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !height[y][x - 1]) {
      height[y][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1]) {
      height[y + 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !height[y - 1][x]) {
      height[y - 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < MAP_Y && !height[y + 1][x]) {
      height[y + 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1]) {
      height[y - 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < MAP_X && !height[y][x + 1]) {
      height[y][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1]) {
      height[y + 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);

  out = fopen("smoothed.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->height, sizeof (m->height), 1, out);
  fclose(out);
  */

  return 0;
}

static void find_building_location(map_t *m, pair_t p)
{
  do {
    p[dim_x] = rand() % (MAP_X - 5) + 3;
    p[dim_y] = rand() % (MAP_Y - 10) + 5;

    if ((((mapxy(p[dim_x] - 1, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] - 1, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x] + 2, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] + 2, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] - 1) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] - 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] + 2) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 2) == ter_path)))   &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_center))) &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_path)))) {
          break;
    }
  } while (1);
}

static int place_pokemart(map_t *m)
{
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_mart;

  return 0;
}

static int place_center(map_t *m)
{  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_center;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_center;

  return 0;
}

static int map_terrain(map_t *m, uint8_t n, uint8_t s, uint8_t e, uint8_t w)
{
  int32_t i, x, y;
  queue_node_t *head, *tail, *tmp;
  FILE *out;
  int num_grass, num_clearing, num_mountain, num_forest, num_total;
  terrain_type_t type;
  int added_current = 0;
  
  num_grass = rand() % 4 + 2;
  num_clearing = rand() % 4 + 2;
  num_mountain = rand() % 2 + 1;
  num_forest = rand() % 2 + 1;
  num_total = num_grass + num_clearing + num_mountain + num_forest;

  memset(&m->map, 0, sizeof (m->map));

  /* Seed with some values */
  for (i = 0; i < num_total; i++) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (m->map[y][x]);
    if (i == 0) {
      type = ter_grass;
    } else if (i == num_grass) {
      type = ter_clearing;
    } else if (i == num_grass + num_clearing) {
      type = ter_mountain;
    } else if (i == num_grass + num_clearing + num_mountain) {
      type = ter_forest;
    }
    m->map[y][x] = type;
    if (i == 0) {
      head = tail = malloc(sizeof (*tail));
    } else {
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);

  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = m->map[y][x];
    
    if (x - 1 >= 0 && !m->map[y][x - 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x - 1] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x - 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y - 1 >= 0 && !m->map[y - 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y - 1][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y - 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y + 1 < MAP_Y && !m->map[y + 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y + 1][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y + 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (x + 1 < MAP_X && !m->map[y][x + 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x + 1] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x + 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = i;
        tail->next = malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    added_current = 0;
    tmp = head;
    head = head->next;
    free(tmp);
  }

  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (y == 0 || y == MAP_Y - 1 ||
          x == 0 || x == MAP_X - 1) {
        mapxy(x, y) = ter_boulder;
      }
    }
  }

  m->n = n;
  m->s = s;
  m->e = e;
  m->w = w;

  mapxy(n,         0        ) = ter_path;
  mapxy(n,         1        ) = ter_path;
  mapxy(s,         MAP_Y - 1) = ter_path;
  mapxy(s,         MAP_Y - 2) = ter_path;
  mapxy(0,         w        ) = ter_path;
  mapxy(1,         w        ) = ter_path;
  mapxy(MAP_X - 1, e        ) = ter_path;
  mapxy(MAP_X - 2, e        ) = ter_path;

  return 0;
}

static int place_boulders(map_t *m)
{
  int i;
  int x, y;

  for (i = 0; i < MIN_BOULDERS || rand() % 100 < BOULDER_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_forest) {
      m->map[y][x] = ter_boulder;
    }
  }

  return 0;
}

static int place_trees(map_t *m)
{
  int i;
  int x, y;
  
  for (i = 0; i < MIN_TREES || rand() % 100 < TREE_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_mountain) {
      m->map[y][x] = ter_tree;
    }
  }

  return 0;
}

void get_exits(map_t *world[WORLD_DIM][WORLD_DIM], int *n,
  int *s, int *e, int *w, int y, int x){

  //check north
  if((y-1 >= 0) && !(world[y-1][x] == NULL) ){
    *n = world[y-1][x]->s;
  }
  else{
    *n = 1 + rand() % (MAP_X - 2);
  }

  //check south
  if((y+1 < WORLD_DIM) && !(world[y+1][x] == NULL) ){
    *s = world[y+1][x]->n;
  }
  else{
    *s = 1 + rand() % (MAP_X - 2);
  }

  //check east
  if((x+1 < WORLD_DIM) && !(world[y][x+1] == NULL) ){
    *e = world[y][x+1]->w;
  }
  else{
    *e = 1 + rand() % (MAP_Y - 2);
  }

  //check west
  if((x-1 >= 0) && !(world[y][x-1] == NULL) ){
    *w = world[y][x-1]->e;
  }
  else{
    *w = 1 + rand() % (MAP_Y - 2);
  }

}

static int new_map(map_t *world[WORLD_DIM][WORLD_DIM],int y, int x)
{
  
  if(world[y][x] == NULL){
    world[y][x] = malloc(sizeof (map_t));
  }

  int n, s, e, w;

  get_exits(world, &n, &s, &e, &w, y, x);

  smooth_height(world[y][x]);
  map_terrain(world[y][x], n, s, e, w);
  place_boulders(world[y][x]);
  place_trees(world[y][x]);
  build_paths(world[y][x]);

  double dx = abs((x-199)*(x-199));
  double dy = abs((y-199)*(y-199));
  double d = sqrt(dy+dx);

  int val = (int)((-45*d/200)+50);

  int p_mart;
  if (rand()%100+1 < val){
    p_mart =1;
  }
  else{
    p_mart =0;
  }

  int p_center;
  if (rand()%100+1 < val){
    p_center =1;
  }
  else{
    p_center =0;
  }


  if(p_mart){
    place_pokemart(world[y][x]);
  }
  if (p_center){
    place_center(world[y][x]);
  }

  //TODO place char on path

  int i;
  for(i=1; i<MAP_Y-1; i++){
    if(world[y][x]->map[i][40] == ter_path){
      world[y][x]->pc[dim_x] = 40;
      world[y][x]->pc[dim_y] = i;
      break;
    }
  }
  

  if(y == 0){
    world[y][x]->map[y][n] = ter_boulder;
  }
  if(x == 0){
    world[y][x]->map[w][x] = ter_boulder;
  } 
  if(x == WORLD_DIM-1){
    world[y][x]->map[e][MAP_X-1] = ter_boulder;
  }
  if (y == WORLD_DIM-1){
    world[y][x]->map[MAP_Y-1][s] = ter_boulder;
  }

  

  return 0;
}

static void print_map(map_t *m)
{

  printf("Hiker Map:\n");
  dijkstra_square(m, m->pc, get_hiker_cost);
  printf("Rival Map:\n");
  dijkstra_square(m, m->pc, get_rival_cost);
  int x, y;
  int default_reached = 0;
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      
      if(m->pc[dim_x]==x && m->pc[dim_y]==y){
        putchar('@');
      }
      else{
        switch (m->map[y][x]) {
        case ter_boulder:
        case ter_mountain:
          putchar('%');
          break;
        case ter_tree:
        case ter_forest:
          putchar('^');
          break;
        case ter_path:
          putchar('#');
          break;
        case ter_mart:
          putchar('M');
          break;
        case ter_center:
          putchar('C');
          break;
        case ter_grass:
          putchar(':');
          break;
        case ter_clearing:
          putchar('.');
          break;
        default:
          default_reached = 1;
          break;
        }
      }
      
    }
    putchar('\n');
  }

  if (default_reached) {
    fprintf(stderr, "Default reached in %s\n", __FUNCTION__);
  }
}

int check_bounds(int y, int x){
  if(y<0 || y>WORLD_DIM-1 || x<0 || x>WORLD_DIM-1){
    return 0;
  }
  return 1;
}


int main(int argc, char *argv[])
{
  //define world array
  map_t *world[WORLD_DIM][WORLD_DIM];


  //initialize to null
  int i,j;
  for (i=0;i<WORLD_DIM;i++){
    for (j=0;j<WORLD_DIM;j++){
      world[i][j] = NULL;
    }
  }


  //seeding
  struct timeval tv;
  uint32_t seed;

  if (argc == 2) {
    seed = atoi(argv[1]);
  } else {
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }
  //TO REMOVE!!
  seed = 2144914724;


  printf("Using seedd: %u\n", seed);
  srand(seed);

  int x = WORLD_CEN;
  int y = WORLD_CEN;

  //print starting world
  new_map(world, y, x);
  print_map(world[y][x]);
  printf("(%d, %d)", x-199, y-199);

  int ty, tx;
  char input;
  printf("\nEnter Command: ");
  scanf(" %c", &input);

  while(!(input == 'q')){
    int valid = 1;
    ty = y;
    tx = x;
    switch(input){
      case 'n':
        ty--;
        break;
      case 's':
        ty++;
        break;
      case 'e':
        tx++;
        break;
      case 'w':
        tx--;
        break;
      case 'f':
        scanf("%d %d", &tx, &ty);
        tx = tx+199;
        ty = ty+199;
        break;
      default:
        valid =0;
        print_map(world[y][x]);
        printf("(%d, %d)", x-199, y-199);
        printf(" Invalid Command! Enter New Command.");
        break;
    }
    if(check_bounds(ty, tx) == 0){
      valid =0;
      print_map(world[y][x]);
      printf("(%d, %d)", x-199, y-199);
      printf(" Out of Bounds! Enter New Command.");
    }
    else {
      y = ty;
      x = tx;
    }

    if(valid){
      if(world[y][x] == NULL){
        new_map(world, y, x);
      }
      print_map(world[y][x]);
      printf("(%d, %d)", x-199, y-199);
    }
    else{
      //printf("Input invalid, please enter a new command.");
    }

    printf("\nEnter Command: ");
    scanf(" %c", &input);
  }

  for (i=0;i<WORLD_DIM;i++){
    for (j=0;j<WORLD_DIM;j++){
      if(world[i][j]!=NULL){
        free(world[i][j]);
      }
    }
  }

  
  return 0;
}
