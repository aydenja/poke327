#include <unistd.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <cstring>

#include "io.h"
#include "character.h"
#include "poke327.h"
#include "pokemon.h"
#include "db_parse.h"

typedef struct io_message {
  /* Will print " --more-- " at end of line when another message follows. *
   * Leave 10 extra spaces for that.                                      */
  char msg[71];
  struct io_message *next;
} io_message_t;

static io_message_t *io_head, *io_tail;

void io_init_terminal(void)
{
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  start_color();
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void io_reset_terminal(void)
{
  endwin();

  while (io_head) {
    io_tail = io_head;
    io_head = io_head->next;
    free(io_tail);
  }
  io_tail = NULL;
}

void io_queue_message(const char *format, ...)
{
  io_message_t *tmp;
  va_list ap;

  if (!(tmp = (io_message_t *) malloc(sizeof (*tmp)))) {
    perror("malloc");
    exit(1);
  }

  tmp->next = NULL;

  va_start(ap, format);

  vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

  va_end(ap);

  if (!io_head) {
    io_head = io_tail = tmp;
  } else {
    io_tail->next = tmp;
    io_tail = tmp;
  }
}

static void io_print_message_queue(uint32_t y, uint32_t x)
{
  while (io_head) {
    io_tail = io_head;
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(y, x, "%-80s", io_head->msg);
    attroff(COLOR_PAIR(COLOR_CYAN));
    io_head = io_head->next;
    if (io_head) {
      attron(COLOR_PAIR(COLOR_CYAN));
      mvprintw(y, x + 70, "%10s", " --more-- ");
      attroff(COLOR_PAIR(COLOR_CYAN));
      refresh();
      getch();
    }
    free(io_tail);
  }
  io_tail = NULL;
}

/**************************************************************************
 * Compares trainer distances from the PC according to the rival distance *
 * map.  This gives the approximate distance that the PC must travel to   *
 * get to the trainer (doesn't account for crossing buildings).  This is  *
 * not the distance from the NPC to the PC unless the NPC is a rival.     *
 *                                                                        *
 * Not a bug.                                                             *
 **************************************************************************/
static int compare_trainer_distance(const void *v1, const void *v2)
{
  const Character *const *c1 = (const Character *const *) v1;
  const Character *const *c2 = (const Character *const *) v2;

  return (world.rival_dist[(*c1)->pos[dim_y]][(*c1)->pos[dim_x]] -
          world.rival_dist[(*c2)->pos[dim_y]][(*c2)->pos[dim_x]]);
}

static Character *io_nearest_visible_trainer()
{
  Character **c, *n;
  uint32_t x, y, count;

  c = (Character **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  n = c[0];

  free(c);

  return n;
}

void io_display()
{
  uint32_t y, x;
  Character *c;

  clear();
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->cmap[y][x]) {
        mvaddch(y + 1, x, world.cur_map->cmap[y][x]->symbol);
      } else {
        switch (world.cur_map->map[y][x]) {
        case ter_boulder:
        case ter_mountain:
          attron(COLOR_PAIR(COLOR_MAGENTA));
          mvaddch(y + 1, x, '%');
          attroff(COLOR_PAIR(COLOR_MAGENTA));
          break;
        case ter_tree:
        case ter_forest:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, '^');
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_path:
        case ter_exit:
          attron(COLOR_PAIR(COLOR_YELLOW));
          mvaddch(y + 1, x, '#');
          attroff(COLOR_PAIR(COLOR_YELLOW));
          break;
        case ter_mart:
          attron(COLOR_PAIR(COLOR_BLUE));
          mvaddch(y + 1, x, 'M');
          attroff(COLOR_PAIR(COLOR_BLUE));
          break;
        case ter_center:
          attron(COLOR_PAIR(COLOR_RED));
          mvaddch(y + 1, x, 'C');
          attroff(COLOR_PAIR(COLOR_RED));
          break;
        case ter_grass:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, ':');
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        case ter_clearing:
          attron(COLOR_PAIR(COLOR_GREEN));
          mvaddch(y + 1, x, '.');
          attroff(COLOR_PAIR(COLOR_GREEN));
          break;
        default:
 /* Use zero as an error symbol, since it stands out somewhat, and it's *
  * not otherwise used.                                                 */
          attron(COLOR_PAIR(COLOR_CYAN));
          mvaddch(y + 1, x, '0');
          attroff(COLOR_PAIR(COLOR_CYAN)); 
       }
      }
    }
  }

  mvprintw(23, 1, "PC position is (%2d,%2d) on map %d%cx%d%c.",
           world.pc.pos[dim_x],
           world.pc.pos[dim_y],
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
  mvprintw(22, 1, "%d known %s.", world.cur_map->num_trainers,
           world.cur_map->num_trainers > 1 ? "trainers" : "trainer");
  mvprintw(22, 30, "Nearest visible trainer: ");
  if ((c = io_nearest_visible_trainer())) {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(22, 55, "%c at %d %c by %d %c.",
             c->symbol,
             abs(c->pos[dim_y] - world.pc.pos[dim_y]),
             ((c->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              'N' : 'S'),
             abs(c->pos[dim_x] - world.pc.pos[dim_x]),
             ((c->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              'W' : 'E'));
    attroff(COLOR_PAIR(COLOR_RED));
  } else {
    attron(COLOR_PAIR(COLOR_BLUE));
    mvprintw(22, 55, "NONE.");
    attroff(COLOR_PAIR(COLOR_BLUE));
  }

  io_print_message_queue(0, 0);

  refresh();
}

uint32_t io_teleport_pc(pair_t dest)
{
  /* Just for fun. And debugging.  Mostly debugging. */

  do {
    dest[dim_x] = rand_range(1, MAP_X - 2);
    dest[dim_y] = rand_range(1, MAP_Y - 2);
  } while (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]                  ||
           move_cost[char_pc][world.cur_map->map[dest[dim_y]]
                                                [dest[dim_x]]] == INT_MAX ||
           world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);

  return 0;
}

static void io_scroll_trainer_list(char (*s)[40], uint32_t count)
{
  uint32_t offset;
  uint32_t i;

  offset = 0;

  while (1) {
    for (i = 0; i < 13; i++) {
      mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
    }
    switch (getch()) {
    case KEY_UP:
      if (offset) {
        offset--;
      }
      break;
    case KEY_DOWN:
      if (offset < (count - 13)) {
        offset++;
      }
      break;
    case 27:
      return;
    }

  }
}

static void io_list_trainers_display(Npc **c,
                                     uint32_t count)
{
  uint32_t i;
  char (*s)[40]; /* pointer to array of 40 char */

  s = (char (*)[40]) malloc(count * sizeof (*s));

  mvprintw(3, 19, " %-40s ", "");
  /* Borrow the first element of our array for this string: */
  snprintf(s[0], 40, "You know of %d trainers:", count);
  mvprintw(4, 19, " %-40s ", s[0]);
  mvprintw(5, 19, " %-40s ", "");

  for (i = 0; i < count; i++) {
    snprintf(s[i], 40, "%16s %c: %2d %s by %2d %s",
             char_type_name[c[i]->ctype],
             c[i]->symbol,
             abs(c[i]->pos[dim_y] - world.pc.pos[dim_y]),
             ((c[i]->pos[dim_y] - world.pc.pos[dim_y]) <= 0 ?
              "North" : "South"),
             abs(c[i]->pos[dim_x] - world.pc.pos[dim_x]),
             ((c[i]->pos[dim_x] - world.pc.pos[dim_x]) <= 0 ?
              "West" : "East"));
    if (count <= 13) {
      /* Handle the non-scrolling case right here. *
       * Scrolling in another function.            */
      mvprintw(i + 6, 19, " %-40s ", s[i]);
    }
  }

  if (count <= 13) {
    mvprintw(count + 6, 19, " %-40s ", "");
    mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
    while (getch() != 27 /* escape */)
      ;
  } else {
    mvprintw(19, 19, " %-40s ", "");
    mvprintw(20, 19, " %-40s ",
             "Arrows to scroll, escape to continue.");
    io_scroll_trainer_list(s, count);
  }

  free(s);
}

static void io_list_trainers()
{
  Character **c;
  uint32_t x, y, count;

  c = (Character **) malloc(world.cur_map->num_trainers * sizeof (*c));

  /* Get a linear list of trainers */
  for (count = 0, y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (world.cur_map->cmap[y][x] && world.cur_map->cmap[y][x] !=
          &world.pc) {
        c[count++] = world.cur_map->cmap[y][x];
      }
    }
  }

  /* Sort it by distance from PC */
  qsort(c, count, sizeof (*c), compare_trainer_distance);

  /* Display it */
  io_list_trainers_display((Npc **)(c), count);
  free(c);

  /* And redraw the map */
  io_display();
}

void io_pokemart()
{
  mvprintw(0, 0, "Welcome to the Pokemart.  Could I interest you in some Pokeballs?");
  refresh();
  getch();
}

void io_pokemon_center()
{
  mvprintw(0, 0, "Welcome to the Pokemon Center.  How can Nurse Joy assist you?");
  refresh();
  getch();
}

int select_move (Pokemon *p){
  clear();
  int i =0;
  mvprintw(0, 0, "Select a move to use:");
  while(p->move_index[i]){
    mvprintw(i+1, 0, "%d - %s", i+1, moves[p->move_index[i]].identifier);
    i++;
  }
  refresh();
  int valid = false;
  int input;
  while(!valid){
    input =getch();
    switch (input){
      case '1':
        return p->move_index[0];
      case '2':
        return p->move_index[1];
      default:
        mvprintw(5, 0, "%c is not a valid input!", input);
    }
  }
  return 1; 
}

int get_npc_move (Pokemon *p){
  int i =0;
  while(p->move_index[i]){
    i++;
  }
  return p->move_index[rand() % i];
}

int get_move_pri(int pcinx, int npcinx, Pokemon *pc, Pokemon *npc){
  int pcpri = moves[pcinx].priority;
  int npcpri = moves[npcinx].priority;

  if(pcpri == npcpri){
    if(pc->get_speed() == npc->get_speed()){
      return 0;
    }
    else if (pc->get_speed() > npc->get_speed()){
      return 0;
    }
    else{
      return 1;
    }
  }
  else if (pcpri > npcpri){
    return 0;
  }
  else {
    return 1;
  }
}

bool hit (int m){
  if(moves[m].accuracy == -1){
    return true;
  }
  if ((rand() % 100) <= moves[m].accuracy){
    return true;
  }
  else{
    return false;
  }
}

int get_damage(Pokemon *p, int m){
  if(hit(m)){
    //clear();
    //mvprintw(11, 0 , "level %d | atk%d def%d bs%d pow%d auc%d", p->get_level(), p->get_atk(), p->get_def(), p->s->base_stat[stat_speed], moves[m].power,moves[m].accuracy);
    int top = (int)(((p->get_level() *2)/(double)5)+2);
    double pow = moves[m].power;
    if (pow == -1){
      pow = 20;
    }
    top = (int)(top * pow * ((double)p->get_atk()/(double)(p->get_def())));
    top = (int)(((double)top /50.0) +2);
    //mvprintw(12, 0, "top after %d", top);
    double crit;
    int tv = p->s->base_stat[stat_speed]/2;
    double stab =1;
    double type =1;
    if ((rand() % 256) < tv){
      crit = 1.5;
    }
    else{
      crit = 1;
    } 
    double random = (rand() % (100 + 1 - 85) + 85)/100.0;
    //mvprintw(13, 0, "top after2 %d %f", (int)((double)top * crit * random * stab * type), random);
    //refresh();
    //getch();
    top = (int)((double)top * crit * random * stab * type);
    if (top <= 0){
      return 1;
    }
    else{
      return top;
    }
  }
  else{
    return 0;
  }

}

int my_max(int x, int y){
  if(x>=y){
    return x;
  }
  else {
    return y;
  }
}

int my_min(int x, int y){
  if(x<=y){
    return x;
  }
  else {
    return y;
  }
}

void take_damage(Pokemon *p, int dam){
  int nhp = p->effective_stat[stat_hp];
  nhp = nhp -dam;
  p->effective_stat[stat_hp] = my_max(0, nhp);

}

void case_1_moves(int pri, Pokemon * pc, Pokemon * npc, int pc_dam, int npc_dam, int pc_move, int npc_move){
  clear();
  if(pri){//npc first
    if(npc_dam >0){
      mvprintw(0, 0, "%s uses %s and it does %d damage!", npc->get_species(), moves[npc_move].identifier, npc_dam);
      mvprintw(2, 0, "Press any key to advance...");
      refresh();
      getch();
      take_damage(pc, npc_dam);
    }
    else{
      mvprintw(0, 0, "%s uses %s and it misses!", npc->get_species(), moves[npc_move].identifier);
      mvprintw(2, 0, "Press any key to advance...");
      refresh();
      getch();
      take_damage(pc, npc_dam);
    }
   

    if(pc->get_hp() > 0) {
      if(pc_dam >0){
        clear();
        mvprintw(0, 0, "%s uses %s and it does %d damage!", pc->get_species(), moves[pc_move].identifier, pc_dam);
        mvprintw(2, 0, "Press any key to advance...");
        refresh();
        getch();
        take_damage(npc, pc_dam);
      }
      else{
        clear();
        mvprintw(0, 0, "%s uses %s and it misses!", pc->get_species(), moves[pc_move].identifier);
        mvprintw(2, 0, "Press any key to advance...");
        refresh();
        getch();
        take_damage(npc, pc_dam);
      }
    }
    

  }
  else{ // player first
    if(pc_dam >0){
      clear();
      mvprintw(0, 0, "%s uses %s and it does %d damage!", pc->get_species(), moves[pc_move].identifier, pc_dam);
      mvprintw(2, 0, "Press any key to advance...");
      refresh();
      getch();
      take_damage(npc, pc_dam);
    }
    else{
      clear();
      mvprintw(0, 0, "%s uses %s and it misses!", pc->get_species(), moves[pc_move].identifier);
      mvprintw(2, 0, "Press any key to advance...");
      refresh();
      getch();
      take_damage(npc, pc_dam);
    }


    if(npc->get_hp() > 0){
      if(npc_dam >0){
        clear();
        mvprintw(0, 0, "%s uses %s and it does %d damage!", npc->get_species(), moves[npc_move].identifier, npc_dam);
        mvprintw(2, 0, "Press any key to advance...");
        refresh();
        getch();
        take_damage(pc, npc_dam);
      }
      else{
        clear();
        mvprintw(0, 0, "%s uses %s and it misses!", npc->get_species(), moves[npc_move].identifier);
        mvprintw(2, 0, "Press any key to advance...");
        refresh();
        getch();
        take_damage(pc, npc_dam);
      }

    }
  }
}

int show_bag(){
  clear();
  mvprintw(0, 0, "Select an item to use:");
  mvprintw(1, 0, "1. Pokeball x%d", world.pc.num_pb);
  mvprintw(2, 0, "2. Potion x%d", world.pc.num_po);
  mvprintw(3, 0, "3. Revive x%d", world.pc.num_rev);
  refresh();
  int valid = false;
  int input;
  while(!valid){
    input =getch();
    switch (input){
      case '1':
      case '2':
      case '3':
        return input;
      default:
        mvprintw(5, 0, "%c is not a valid input!", input);
    }
  }
  return 0;

}

void case_2_move(int npc_dam, int npc_move, Pokemon * npc, Pokemon * pc){
  clear();
  if(npc_dam >0){
      mvprintw(0, 0, "%s uses %s and it does %d damage!", npc->get_species(), moves[npc_move].identifier, npc_dam);
      mvprintw(2, 0, "Press any key to advance...");
      refresh();
      getch();
      take_damage(pc, npc_dam);
    }
    else{
      mvprintw(0, 0, "%s uses %s and it misses!", npc->get_species(), moves[npc_move].identifier);
      mvprintw(2, 0, "Press any key to advance...");
      refresh();
      getch();
      take_damage(pc, npc_dam);
    }
}

void use_potion(Pokemon *p){
  clear();
  int ihp = p->get_hp();
  int nhp = my_min(p->start_hp, ihp+20);

  p->effective_stat[stat_hp] = nhp;

  mvprintw(0, 0, "You used a potion on %s, their hp went from %d -> %d!", p->get_species(), ihp, p->get_hp());
  mvprintw(2, 0, "Press any key to contine...");

  refresh();
  getch();

}

void get_oofed_pokemon(std::vector<Pokemon *> &v){
  while(v.size()){
    v.pop_back();
  }
  int i;
  for(i=0; i<(int)world.pc.poke.size(); i++){
    if(world.pc.poke[i]->get_hp() == 0){
      v.push_back(world.pc.poke[i]);
    }
  }
}

void get_ava_pokemon(std::vector<Pokemon *> &v){
  while(v.size()){
    v.pop_back();
  }
  int i;
  for(i=0; i<(int)world.pc.poke.size(); i++){
    if(world.pc.poke[i]->get_hp() > 0){
      v.push_back(world.pc.poke[i]);
    }
  }
}

void call_revive(Pokemon *p){
  clear();
  int ihp = p->get_hp();
  int nhp = p->start_hp /2;

  p->effective_stat[stat_hp] = nhp;

  mvprintw(0, 0, "You used a revive on %s, their hp went from %d -> %d!", p->get_species(), ihp, p->get_hp());
  mvprintw(2, 0, "Press any key to contine...");

  refresh();
  getch();
}

Pokemon * switch_poke(std::vector<Pokemon *> &v){
  clear();
  mvprintw(0, 0, "Select a pokemon to use:");
  int i;
  for(i=0; i<(int)v.size(); i++){
    mvprintw(i+1, 0, "%d. %s", i+1, v[i]->get_species());
  }
  refresh();
  int input = getchar();
  int a =-1;
  while(a == -1){
    refresh();
    switch (input){
      case '1':
        if(v.size()>= 1){
          a =0;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;
      
      case '2':
        if(v.size()>=2){
          a =1;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;

      case '3':
        if(v.size()>= 3){
          a =2;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;

      case '4':
        if(v.size()>= 4){
          a =3;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;

      case '5':
        if(v.size()>= 5){
          a =4;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;

      case '6':
        if(v.size()>= 6){
          a =5;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;

      
    }
  }

  return v[a];
  
}

void use_revive(std::vector<Pokemon *> &v){
  clear();
  mvprintw(0, 0, "Select a pokemon to revive:");
  int i;
  for(i=0; i<(int)v.size(); i++){
    mvprintw(i+1, 0, "%d. %s", i+1, v[i]->get_species());
  }
  refresh();
  int input = getchar();
  int a =-1;
  while(a == -1){
    refresh();
    switch (input){
      case '1':
        if(v.size()>= 1){
          a =0;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;
      
      case '2':
        if(v.size()>=2){
          a =1;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;

      case '3':
        if(v.size()>= 3){
          a =2;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;

      case '4':
        if(v.size()>= 4){
          a =3;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;

      case '5':
        if(v.size()>= 5){
          a =4;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;

      case '6':
        if(v.size()>= 6){
          a =5;
        }
        else{
          mvprintw (8, 0, "Not a valid input!");
          refresh();
          input = getchar();
        }
        break;

      
    }
  }

  call_revive(v[a]);
  
}

void io_battle(Character *aggressor, Character *defender)
{
  Npc *npc;
  if (!(npc = dynamic_cast<Npc *>(aggressor))) {
    npc = dynamic_cast<Npc *>(defender);
  }

  clear();
  
  mvprintw(0,0,"You have entered a battle with %c!", npc->symbol);
  mvprintw(1,0, "Press any key to continue...");
  getch();

  clear();
  Pokemon *pc_cp = world.pc.get_next();
  Pokemon *npc_cp = npc->get_next();

  while( (!npc->is_done()) && (!world.pc.is_done())){
    if(npc_cp->get_hp() == 0){
      char old[40];
      strcpy(old, npc_cp->get_species());
      npc_cp = npc->get_next();
      clear();
      mvprintw(0,0,"You knocked out %s, %s enters the battle!", old, npc_cp->get_species());
      mvprintw(1,0, "Press any key to continue...");
      refresh();
      getch();
    }
    

    if(pc_cp->get_hp() == 0){
      clear();
      mvprintw(0,0,"%s has fainted!", pc_cp->get_species());
      mvprintw(1,0, "Press any key to continue...");
      refresh();
      getch();
      std::vector<Pokemon *> w;
      get_ava_pokemon(w);
      pc_cp = switch_poke(w);
    }
    
    clear();
    mvprintw(0,0, "Opponent's Pokemon:");
    mvprintw(1,0,"%s LV:%d", npc_cp->get_species(), npc_cp->get_level());
    mvprintw(2,0, "HP: %d", npc_cp->get_hp());

    mvprintw(4,0, "Your Pokemon:");
    mvprintw(5,0,"%s LV:%d", pc_cp->get_species(), pc_cp->get_level());
    mvprintw(6,0, "HP: %d", pc_cp->get_hp());

    mvprintw(8,0, "Select an option:");
    mvprintw(9,0, "1 - Fight   2 - Bag");
    mvprintw(10,0, "3 - Run     4 - Pokemon");
    refresh();
    int input;
    input = getch();
    int pc_move;
    int npc_move;
    int mp;
    int pc_dam;
    int npc_dam;
    switch (input){
      case '1':
        pc_move = select_move(pc_cp);
        npc_move = get_npc_move(npc_cp);
        mp = get_move_pri(pc_move, npc_move, pc_cp, npc_cp);
        pc_dam = get_damage(pc_cp, pc_move);
        npc_dam = get_damage(npc_cp, npc_move);
        case_1_moves(mp, pc_cp, npc_cp, pc_dam, npc_dam, pc_move, npc_move);
        break;
      case '2':
        int item;
        item = show_bag();
        npc_move = get_npc_move(npc_cp);
        npc_dam = get_damage(npc_cp, npc_move);
        switch (item){
          case '1':
            clear();
            mvprintw(0, 0, "You cant use a pokeball in a trainer battle");
            mvprintw(1, 0, "Press any key to continue...");
            refresh();
            getch();
            break;
          case '2':
            if(world.pc.num_po >0){
              use_potion(pc_cp);
              case_2_move(npc_dam, npc_move, npc_cp, pc_cp);
              world.pc.num_po--;
            }
            else{
              clear();
              mvprintw(0, 0, "You do not have enough potions!");
              mvprintw(1, 0, "Press any key to continue...");
              refresh();
              getch();
            }
            break;
          case '3':
            
            if(world.pc.num_rev >0){
              std::vector<Pokemon *> v;
              get_oofed_pokemon(v);
              if((int)v.size() > 0){
                use_revive(v);
                world.pc.num_rev--;
              }
              else{
                clear();
                mvprintw(0, 0, "None of your pokemon have fainted!");
                mvprintw(1, 0, "Press any key to continue...");
                refresh();
                getch();
              }
              
            }
            else{
              clear();
              mvprintw(0, 0, "You do not have enough revives!");
              mvprintw(1, 0, "Press any key to continue...");
              refresh();
              getch();
            }
            break;
          default:
            break; 
        }
        break;
      case '3':
        clear();
        mvprintw(0, 0, "You cant escape from a trainer battle!");
        mvprintw(1, 0, "Press any key to continue...");
        refresh();
        getch();
        break;
      case '4':
      {
        clear();
        mvprintw(0,0,"You brought %s back!", pc_cp->get_species());
        mvprintw(1,0, "Press any key to continue...");
        refresh();
        getch();
        std::vector<Pokemon *> l;
        get_ava_pokemon(l);
        pc_cp = switch_poke(l);
        npc_move = get_npc_move(npc_cp);
        npc_dam = get_damage(npc_cp, npc_move);
        case_2_move(npc_dam, npc_move, npc_cp, pc_cp);
        break;
      }
      case 'q': //REMOVE
        goto exitloop;
        break;
      default:
        mvprintw(13, 0, "%c is not a valid input!", input);
        refresh();
        break;
    }


  }

  exitloop: 

  io_display();
  if(!world.pc.is_done()){
    mvprintw(0, 0, "Aww, how'd you get so strong?  You and your pokemon must share a special bond!");
  }
  else {
    mvprintw(0, 0, "What's important is that you never give up, even if you lose!");
  }
  
  refresh();
  getch();
  
  
  npc->defeated = 1;
  if (npc->ctype == char_hiker || npc->ctype == char_rival) {
    npc->mtype = move_wander;
  }
}

uint32_t move_pc_dir(uint32_t input, pair_t dest)
{
  dest[dim_y] = world.pc.pos[dim_y];
  dest[dim_x] = world.pc.pos[dim_x];

  switch (input) {
  case 1:
  case 2:
  case 3:
    dest[dim_y]++;
    break;
  case 4:
  case 5:
  case 6:
    break;
  case 7:
  case 8:
  case 9:
    dest[dim_y]--;
    break;
  }
  switch (input) {
  case 1:
  case 4:
  case 7:
    dest[dim_x]--;
    break;
  case 2:
  case 5:
  case 8:
    break;
  case 3:
  case 6:
  case 9:
    dest[dim_x]++;
    break;
  case '>':
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_mart) {
      io_pokemart();
    }
    if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ==
        ter_center) {
      io_pokemon_center();
    }
    break;
  }

  if ((world.cur_map->map[dest[dim_y]][dest[dim_x]] == ter_exit) &&
      (input == 1 || input == 3 || input == 7 || input == 9)) {
    // Exiting diagonally leads to complicated entry into the new map
    // in order to avoid INT_MAX move costs in the destination.
    // Most easily solved by disallowing such entries here.
    return 1;
  }

  if (world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) {
    if (dynamic_cast<Npc *>(world.cur_map->cmap[dest[dim_y]][dest[dim_x]]) &&
        ((Npc *) world.cur_map->cmap[dest[dim_y]][dest[dim_x]])->defeated) {
      // Some kind of greeting here would be nice
      return 1;
    } else if (dynamic_cast<Npc *>
               (world.cur_map->cmap[dest[dim_y]][dest[dim_x]])) {
      io_battle(&world.pc, world.cur_map->cmap[dest[dim_y]][dest[dim_x]]);
      // Not actually moving, so set dest back to PC position
      dest[dim_x] = world.pc.pos[dim_x];
      dest[dim_y] = world.pc.pos[dim_y];
    }
  }
  
  if (move_cost[char_pc][world.cur_map->map[dest[dim_y]][dest[dim_x]]] ==
      INT_MAX) {
    return 1;
  }

  return 0;
}

void io_teleport_world(pair_t dest)
{
  int x, y;
  
  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;

  mvprintw(0, 0, "Enter x [-200, 200]: ");
  refresh();
  echo();
  curs_set(1);
  mvscanw(0, 21, (char *) "%d", &x);
  mvprintw(0, 0, "Enter y [-200, 200]:          ");
  refresh();
  mvscanw(0, 21, (char *) "%d", &y);
  refresh();
  noecho();
  curs_set(0);

  if (x < -200) {
    x = -200;
  }
  if (x > 200) {
    x = 200;
  }
  if (y < -200) {
    y = -200;
  }
  if (y > 200) {
    y = 200;
  }
  
  x += 200;
  y += 200;

  world.cur_idx[dim_x] = x;
  world.cur_idx[dim_y] = y;

  new_map(1);
  io_teleport_pc(dest);
}

bool get_escape_odds(Pokemon * pc, Pokemon * npc, int att){
  double top = (double)pc->effective_stat[stat_speed] * 32.0;
  double bot = (double)((int)((double)npc->effective_stat[stat_speed]/4.0) % 256);
  int boost = 30*att;
  top = top/bot;
  int final = top +boost;

  if((rand()%256) <= final){
    return true;
  }
  else {
    return false;
  }

}

void io_encounter_pokemon()
{
  Pokemon *npc_cp;
  
  int md = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
            abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)));
  int minl, maxl;
  
  if (md <= 200) {
    minl = 1;
    maxl = md / 2;
  } else {
    minl = (md - 200) / 2;
    maxl = 100;
  }
  if (minl < 1) {
    minl = 1;
  }
  if (minl > 100) {
    minl = 100;
  }
  if (maxl < 1) {
    maxl = 1;
  }
  if (maxl > 100) {
    maxl = 100;
  }

  npc_cp = new Pokemon(rand() % (maxl - minl + 1) + minl);

  clear();
  
  mvprintw(0,0,"A wild %s appears!", npc_cp->get_species());
  mvprintw(1,0, "Press any key to continue...");
  refresh();
  getch();

  clear();
  Pokemon *pc_cp = world.pc.get_next();
  bool caught = false;
  bool escape = false;
  int att =1;

  while( (npc_cp->get_hp() > 0) && (!world.pc.is_done()) && !caught && !escape){
    
    if(pc_cp->get_hp() == 0){
      clear();
      mvprintw(0,0,"%s has fainted!", pc_cp->get_species());
      mvprintw(1,0, "Press any key to continue...");
      refresh();
      getch();
      std::vector<Pokemon *> w;
      get_ava_pokemon(w);
      pc_cp = switch_poke(w);
    }
    
    
    clear();
    mvprintw(0,0, "Wild Pokemon:");
    mvprintw(1,0,"%s LV:%d", npc_cp->get_species(), npc_cp->get_level());
    mvprintw(2,0, "HP: %d", npc_cp->get_hp());

    mvprintw(4,0, "Your Pokemon:");
    mvprintw(5,0,"%s LV:%d", pc_cp->get_species(), pc_cp->get_level());
    mvprintw(6,0, "HP: %d", pc_cp->get_hp());

    mvprintw(8,0, "Select an option:");
    mvprintw(9,0, "1 - Fight   2 - Bag");
    mvprintw(10,0, "3 - Run     4 - Pokemon");
    refresh();
    int input;
    input = getch();
    int pc_move;
    int npc_move;
    int mp;
    int pc_dam;
    int npc_dam;
    switch (input){
      case '1':
        pc_move = select_move(pc_cp);
        npc_move = get_npc_move(npc_cp);
        mp = get_move_pri(pc_move, npc_move, pc_cp, npc_cp);
        pc_dam = get_damage(pc_cp, pc_move);
        npc_dam = get_damage(npc_cp, npc_move);
        case_1_moves(mp, pc_cp, npc_cp, pc_dam, npc_dam, pc_move, npc_move);
        break;
      case '2':
        int item;
        item = show_bag();
        npc_move = get_npc_move(npc_cp);
        npc_dam = get_damage(npc_cp, npc_move);
        switch (item){
          case '1':
            if(world.pc.num_pb >0){
              world.pc.num_pb--;
              clear();
              mvprintw(0, 0, "You used a Pokeball on %s!", npc_cp->get_species());
              mvprintw(1, 0, "Press any key to continue...");
              refresh();
              getch();
              caught = true;
            }
            else {
              clear();
              mvprintw(0, 0, "You do not have enough pokeballs!");
              mvprintw(1, 0, "Press any key to continue...");
              refresh();
              getch();
            }
            break;
          case '2':
            if(world.pc.num_po >0){
              use_potion(pc_cp);
              case_2_move(npc_dam, npc_move, npc_cp, pc_cp);
              world.pc.num_po--;
            }
            else{
              clear();
              mvprintw(0, 0, "You do not have enough potions!");
              mvprintw(1, 0, "Press any key to continue...");
              refresh();
              getch();
            }
            break;
          case '3':
            
            if(world.pc.num_rev >0){
              std::vector<Pokemon *> v;
              get_oofed_pokemon(v);
              if((int)v.size() > 0){
                use_revive(v);
                world.pc.num_rev--;
              }
              else{
                clear();
                mvprintw(0, 0, "None of your pokemon have fainted!");
                mvprintw(1, 0, "Press any key to continue...");
                refresh();
                getch();
              }
              
            }
            else{
              clear();
              mvprintw(0, 0, "You do not have enough revives!");
              mvprintw(1, 0, "Press any key to continue...");
              refresh();
              getch();
            }
            break;
          default:
            break; 
        }
        break;
      case '3':
        bool did;
        did = get_escape_odds(pc_cp,npc_cp,att);
        if(did == false){
          clear();
          mvprintw(0, 0, "You tried to escape but could not get away!");
          mvprintw(1, 0, "Press any key to continue...");
          refresh();
          getch();
          npc_move = get_npc_move(npc_cp);
          npc_dam = get_damage(npc_cp, npc_move);
          case_2_move(npc_dam, npc_move, npc_cp, pc_cp);
        }
        else {
          escape = true;
        }
        
        att++;
        break;
      case '4':
        {
        clear();
        mvprintw(0,0,"You brought %s back!", pc_cp->get_species());
        mvprintw(1,0, "Press any key to continue...");
        refresh();
        getch();
        std::vector<Pokemon *> l;
        get_ava_pokemon(l);
        pc_cp = switch_poke(l);
        npc_move = get_npc_move(npc_cp);
        npc_dam = get_damage(npc_cp, npc_move);
        case_2_move(npc_dam, npc_move, npc_cp, pc_cp);
        break;
      }
        break;
      case 'q': //REMOVE
        caught = true;
      default:
        mvprintw(13, 0, "%c is not a valid input!", input);
        break;
    }

  }

  bool del = true;
  Pokemon * p = npc_cp;
  if (caught){
    if((int)world.pc.poke.size() < 6){
      clear();
      mvprintw(0, 0, "You caught %s!", npc_cp->get_species());
      mvprintw(1, 0, "Press any key to continue...");
      getch();
      world.pc.poke.push_back(npc_cp);
      del = false;

      
      io_queue_message("%s%s%s: HP:%d ATK:%d DEF:%d SPATK:%d SPDEF:%d SPEED:%d %s",
                      p->is_shiny() ? "*" : "", p->get_species(),
                      p->is_shiny() ? "*" : "", p->get_hp(), p->get_atk(),
                      p->get_def(), p->get_spatk(), p->get_spdef(),
                      p->get_speed(), p->get_gender_string());
      io_queue_message("%s's moves: %s %s", p->get_species(),
                      p->get_move(0), p->get_move(1));
    }
    else {
      clear();
      mvprintw(0, 0, "You caught %s but did not have enough space! \n%s was released back into the wild.", npc_cp->get_species(),  npc_cp->get_species());
      mvprintw(2, 0, "Press any key to continue...");
      refresh();
      getch();
    }

  }
  else if (!escape) {
    clear();
    mvprintw(0, 0, "You were defeated by %s", npc_cp->get_species());
    mvprintw(2, 0, "Press any key to continue...");
    refresh();
    getch();
  }
  else {
    clear();
    mvprintw(0, 0, "You escaped from %s!", npc_cp->get_species());
    mvprintw(2, 0, "Press any key to continue...");
    refresh();
    getch();
  }
  
  io_display();

  // Later on, don't delete if captured
  if(del){
    delete p;
  }
  
  
}

void io_handle_input(pair_t dest)
{
  uint32_t turn_not_consumed;
  int key;

  do {
    switch (key = getch()) {
    case '7':
    case 'y':
    case KEY_HOME:
      turn_not_consumed = move_pc_dir(7, dest);
      break;
    case '8':
    case 'k':
    case KEY_UP:
      turn_not_consumed = move_pc_dir(8, dest);
      break;
    case '9':
    case 'u':
    case KEY_PPAGE:
      turn_not_consumed = move_pc_dir(9, dest);
      break;
    case '6':
    case 'l':
    case KEY_RIGHT:
      turn_not_consumed = move_pc_dir(6, dest);
      break;
    case '3':
    case 'n':
    case KEY_NPAGE:
      turn_not_consumed = move_pc_dir(3, dest);
      break;
    case '2':
    case 'j':
    case KEY_DOWN:
      turn_not_consumed = move_pc_dir(2, dest);
      break;
    case '1':
    case 'b':
    case KEY_END:
      turn_not_consumed = move_pc_dir(1, dest);
      break;
    case '4':
    case 'h':
    case KEY_LEFT:
      turn_not_consumed = move_pc_dir(4, dest);
      break;
    case '5':
    case ' ':
    case '.':
    case KEY_B2:
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case '>':
      turn_not_consumed = move_pc_dir('>', dest);
      break;
    case 'Q':
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      world.quit = 1;
      turn_not_consumed = 0;
      break;
      break;
    case 't':
      /* Teleport the PC to a random place in the map.              */
      io_teleport_pc(dest);
      turn_not_consumed = 0;
      break;
    case 'T':
      /* Teleport the PC to any map in the world.                   */
      io_teleport_world(dest);
      turn_not_consumed = 0;
      break;
    case 'm':
      io_list_trainers();
      turn_not_consumed = 1;
      break;
    case 'B':
      int item;
      item = show_bag();
      switch (item){
        case '1':
          clear();
          mvprintw(0, 0, "You cant use a pokeball when you are not in a battle!");
          mvprintw(1, 0, "Press any key to continue...");
          getch();
          break;
        case '2':
          if(world.pc.num_po >0){
            std::vector<Pokemon *> l;
            get_ava_pokemon(l);
            Pokemon * pc_cp = switch_poke(l);
            use_potion(pc_cp);
            world.pc.num_po--;
          }
          else{
            clear();
            mvprintw(0, 0, "You do not have enough potions!");
            mvprintw(1, 0, "Press any key to continue...");
            getch();
          }
          break;
        case '3':
          
          if(world.pc.num_rev >0){
            std::vector<Pokemon *> v;
            get_oofed_pokemon(v);
            if((int)v.size() > 0){
              use_revive(v);
              world.pc.num_rev--;
            }
            else{
              clear();
              mvprintw(0, 0, "None of your pokemon have fainted!");
              mvprintw(1, 0, "Press any key to continue...");
              getch();
            }
            
          }
          else{
            clear();
            mvprintw(0, 0, "You do not have enough revives!");
            mvprintw(1, 0, "Press any key to continue...");
            getch();
          }
          break;
        default:
          break; 
      }
      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    case 'q':
      /* Demonstrate use of the message queue.  You can use this for *
       * printf()-style debugging (though gdb is probably a better   *
       * option.  Not that it matters, but using this command will   *
       * waste a turn.  Set turn_not_consumed to 1 and you should be *
       * able to figure out why I did it that way.                   */
      io_queue_message("This is the first message.");
      io_queue_message("Since there are multiple messages, "
                       "you will see \"more\" prompts.");
      io_queue_message("You can use any key to advance through messages.");
      io_queue_message("Normal gameplay will not resume until the queue "
                       "is empty.");
      io_queue_message("Long lines will be truncated, not wrapped.");
      io_queue_message("io_queue_message() is variadic and handles "
                       "all printf() conversion specifiers.");
      io_queue_message("Did you see %s?", "what I did there");
      io_queue_message("When the last message is displayed, there will "
                       "be no \"more\" prompt.");
      io_queue_message("Have fun!  And happy printing!");
      io_queue_message("Oh!  And use 'Q' to quit!");

      dest[dim_y] = world.pc.pos[dim_y];
      dest[dim_x] = world.pc.pos[dim_x];
      turn_not_consumed = 0;
      break;
    default:
      /* Also not in the spec.  It's not always easy to figure out what *
       * key code corresponds with a given keystroke.  Print out any    *
       * unhandled key here.  Not only does it give a visual error      *
       * indicator, but it also gives an integer value that can be used *
       * for that key in this (or other) switch statements.  Printed in *
       * octal, with the leading zero, because ncurses.h lists codes in *
       * octal, thus allowing us to do reverse lookups.  If a key has a *
       * name defined in the header, you can use the name here, else    *
       * you can directly use the octal value.                          */
      mvprintw(0, 0, "Unbound key: %#o ", key);
      turn_not_consumed = 1;
    }
    refresh();
  } while (turn_not_consumed);
}
