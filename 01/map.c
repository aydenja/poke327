#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define	WIDTH 80
#define HEIGHT 21


char map [HEIGHT][WIDTH];


void fill_border (){
	int i;
	int j;
	for (i=0; i<HEIGHT; i++){
		for (j=0; j<WIDTH; j++){
			if(i==0 || i==HEIGHT-1 || j==0 || j == WIDTH-1){
				map[i][j] = '%';
			}
			else{
				int r = rand()%15;
				if (r==1){
					map[i][j] = '^';
				}
				else if (r==2){ 
					map[i][j] = '%';
				}
				else{
					map[i][j] = '.';
				}
			}
		}
	}
}

void fill_grass (){
	int seed_x1 = (rand() % WIDTH/2) + WIDTH/4;
	int seed_y1 = (rand() % HEIGHT/2) + HEIGHT/4;
	int seed_x2 = (rand() % WIDTH/2) + WIDTH/4;
	int seed_y2 = (rand() % HEIGHT/2) + HEIGHT/4;

	int seed_x3 = (rand() % WIDTH/2) + WIDTH/4;
	int seed_y3 = (rand() % HEIGHT/2) + HEIGHT/4;
	int seed_x4 = (rand() % WIDTH/2) + WIDTH/4;
	int seed_y4 = (rand() % HEIGHT/2) + HEIGHT/4;

	//minimize overlap
	while ((abs(seed_x1-seed_x2)<20) || (abs(seed_y1-seed_y2)<5)) {
		seed_x1 = (rand() % WIDTH/2) + WIDTH/4;
		seed_x2 = (rand() % WIDTH/2) + WIDTH/4;
		seed_y1 = (rand() % HEIGHT/2) + HEIGHT/4;
		seed_y2 = (rand() % HEIGHT/2) + HEIGHT/4;
	}
	while ((abs(seed_x3-seed_x4)<25) || (abs(seed_y3-seed_y4)<8)) {
		seed_x3 = (rand() % WIDTH/2) + WIDTH/4;
		seed_x4 = (rand() % WIDTH/2) + WIDTH/4;
		seed_y3 = (rand() % HEIGHT/2) + HEIGHT/4;
		seed_y4 = (rand() % HEIGHT/2) + HEIGHT/4;
	}


	int i;
	int j;
	for (i=-8; i<8; i++){
		for (j=-20; j<20; j++){
			if(seed_y3+i>0 && seed_y3+i<HEIGHT-1
				&& seed_x3+j>0 && seed_x3+j<WIDTH-1){
				map[seed_y3+i][seed_x3+j] = '.';
			}
			if(seed_y4+i>0 && seed_y4+i<HEIGHT-1
				&& seed_x4+j>0 && seed_x4+j<WIDTH-1){
				map[seed_y4+i][seed_x4+j] = '.';
			}
		}
	}
	for (i=-4; i<4; i++){
		for (j=-15; j<15; j++){
			map[seed_y1+i][seed_x1+j] = ':';
			map[seed_y2+i][seed_x2+j] = ':';
		}
	}

}

int f_min (int a, int b){
	return (a > b) ? b : a;
}
int f_max (int a, int b){
	return (a < b) ? b : a;
}

void fill_path(){
	//horizontal path seeds
	int start_h = (rand() % HEIGHT/2) + HEIGHT/4;
	int end_h = (rand() % HEIGHT/2) + HEIGHT/4;
	int mid_h = (rand() % WIDTH/2) + WIDTH/4;

	while (abs(start_h-end_h)<3){
		start_h = (rand() % HEIGHT/2) + HEIGHT/4;
		end_h = (rand() % HEIGHT/2) + HEIGHT/4;
	}

	map[start_h][0] = '#';
	map[end_h][WIDTH-1] = '#';

	int i;
	for(i=1; i<mid_h; i++){
		map[start_h][i] = '#';
	}
	for(i=WIDTH-1; i>mid_h; i--){
		map[end_h][i] = '#';
	}

	int min = f_min(start_h,end_h);
	int max = f_max(start_h, end_h);
	for(i=min; i<max+1; i++){
		map[i][mid_h] = '#';
	}

	//now do the same for vert path
	int start_v = (rand() % WIDTH/2) + WIDTH/4;
	int end_v = (rand() % WIDTH/2) + WIDTH/4;
	int mid_v = (rand() % HEIGHT/2) + HEIGHT/4;

	while (abs(start_v-end_v)<15){
		start_v = (rand() % WIDTH/2) + WIDTH/4;
		end_v = (rand() % WIDTH/2) + WIDTH/4;
	}

	map[0][start_v] = '#';
	map[HEIGHT-1][end_v] = '#';

	for(i=1; i<mid_v; i++){
		map[i][start_v] = '#';
	}
	for(i=HEIGHT-1; i>mid_v; i--){
		map[i][end_v] = '#';
	}

	min = f_min(start_v,end_v);
	max = f_max(start_v, end_v);
	for(i=min; i<max+1; i++){
		map[mid_v][i] = '#';
	}

	//placement of buildings
	int mart_x = (rand() % (mid_h-8)) + 4;
	int mart_y = start_h-1;


	while((map[mart_y][mart_x] == '#') || (map[mart_y-1][mart_x] == '#') ||
		(map[mart_y][mart_x-1] == '#') || (map[mart_y-1][mart_x-1] == '#')){
		mart_x = (rand() % (mid_h-8)) + 4;
	}
	map[mart_y][mart_x] = 'M';
	map[mart_y-1][mart_x] = 'M';
	map[mart_y][mart_x-1] = 'M';
	map[mart_y-1][mart_x-1] = 'M';

	int cen_x = (rand() % ((WIDTH-mid_h) -8)) + 4  + mid_h;
	int cen_y = end_h - 1;

	while((map[cen_y][cen_x] == '#') || (map[cen_y-1][cen_x] == '#') ||
		(map[cen_y][cen_x-1] == '#') || (map[cen_y-1][cen_x-1] == '#')){
		cen_x = (rand() % ((WIDTH-mid_h) -8)) + 4  + mid_h;
	}

	map[cen_y][cen_x] = 'C';
	map[cen_y-1][cen_x] = 'C';
	map[cen_y][cen_x-1] = 'C';
	map[cen_y-1][cen_x-1] = 'C';
}


void print_map (){
	int i;
	int j;
	for (i=0; i<HEIGHT; i++){
		for (j=0; j<WIDTH; j++){
			printf("%c", map[i][j]);
		}
		printf("\n");
	}
}


int main(int argc, char const *argv[])
{
	srand(time(NULL));
	fill_border();
	fill_grass();
	fill_path();
	print_map();

	return 0;
}