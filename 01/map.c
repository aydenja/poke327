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
				int r = rand()%50;
				if (r==1){
					map[i][j] = '^';
				}
				else if (r==2){ 
					map[i][j] = '%';
				}
				else{
					map[i][j] = ' ';
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
		for (j=-10; j<10; j++){
			map[seed_y1+i][seed_x1+j] = ',';
			map[seed_y2+i][seed_x2+j] = ',';
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
	int start = (rand() % HEIGHT/2) + HEIGHT/4;
	int end = (rand() % HEIGHT/2) + HEIGHT/4;
	int mid = (rand() % WIDTH/2) + WIDTH/4;

	while (abs(start-end)<3){
		start = (rand() % HEIGHT/2) + HEIGHT/4;
		end = (rand() % HEIGHT/2) + HEIGHT/4;
	}

	map[start][0] = '#';
	map[end][WIDTH-1] = '#';

	int i;
	for(i=1; i<mid; i++){
		map[start][i] = '#';
	}
	for(i=WIDTH-1; i>mid; i--){
		map[end][i] = '#';
	}

	int min = f_min(start,end);
	int max = f_max(start, end);
	for(i=min; i<max+1; i++){
		map[i][mid] = '#';
	}

	//now do the same for vert path
	start = (rand() % WIDTH/2) + WIDTH/4;
	end = (rand() % WIDTH/2) + WIDTH/4;
	mid = (rand() % HEIGHT/2) + HEIGHT/4;

	while (abs(start-end)<3){
		start = (rand() % WIDTH/2) + WIDTH/4;
		end = (rand() % WIDTH/2) + WIDTH/4;
	}



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