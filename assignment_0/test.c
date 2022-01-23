#include <stdio.h>
#include <stdlib.h>
#define LENGTH 5
#define WIDTH 5

/*
Defines the possible set of moves for a knight at a given space
*/
int xMove[8] = { 2, 1, -1, -2, -2, -1, 1, 2 };
int yMove[8] = { 1, 2, 2, 1, -1, -2, -2, -1 }; 

int count = 0;


int possibleMove (int y, int x, int path[25], int pLength){
	//If the move is within the bounds, and has not been visited
	if((y >= 0 && y < LENGTH) && (x>=0 && x < WIDTH) ){
		int i;
		int pos = (y*5) + x;
		for (i=0; i<pLength; i++){
			if (path[i] == pos){
				return 0;
			}
		}
		return 1;
	}
	else{
		return 0;
	}
}

void printOutput (int path [25]){
	int i;
	for (i=0; i<24; i++){
		printf("%d, ", path[i]+1);
	}
	printf("%d\n", path[24]+1);
}


void findTours (int  path [25], int posY, int posX, int pLength) {
	//We found a solution once path length is 25
	if(pLength == 25){
		printOutput(path);
		//TODO: Remove in final version.
		count++;
		printf("%d\n", count);
	}
	else{
		int i;
		for(i=0; i<8; i++){
			int nextY = posY + yMove[i];
			int nextX = posX + xMove[i];
			if(possibleMove(nextY, nextX, path, pLength) == 1){
				path[pLength] = (nextY*5) + nextX;
				findTours(path, nextY, nextX, pLength+1);
			}
		}
	}

}


int main(int argc, char const *argv[]){
	int i;
	int j;
	int path [25];
	
	for (i=0; i<LENGTH; i++){
		for (j=0; j<WIDTH; j++){
			path[0] = (i*5) + j;
			findTours(path, i, j, 1);
		}
	}
	return 0;
}

