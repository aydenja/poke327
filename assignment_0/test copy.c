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

int hasMove (int posY, int posX, int path[25], int pLength){
	int i;
	for(i=0; i<8; i++){
		if(possibleMove(posY + yMove[i], posX + xMove[i], path, pLength) == 1){
			return 1;
		}
	}
	return 0;
}

void printOutput (int path [25]){
	int i;
	for (i=0; i<25; i++){
		printf("%d, ", path[i]+1);
	}
	printf("GOOD\n");
}


void findTours (int  visited [5][5], int  path [25],
 int posY, int posX, int pLength) {
	
	//printf("visited %d, %d\nPath:", posY, posX);
	visited[posY][posX] = 1;

	if(hasMove(posY, posX, path, pLength) == 0){
		
		//printf("found\n");
		if (pLength == 25){
			printOutput(path);
			count++;
			printf("%d \n", count);
		}
		else{
			return; 
		}
	}
	else{
		int i;
		for(i=0; i<8; i++){
			int nextY = posY + yMove[i];
			int nextX = posX + xMove[i];
			if(possibleMove(nextY, nextX, path, pLength) == 1){
				path[pLength] = (nextY*5) + nextX;
				//printf("%d\n", path[pLength]);
				findTours(visited, path, nextY, nextX, pLength+1);
				//printf("back\n");
			}
		}
	}

}


int main(int argc, char const *argv[]){
	int i;
	int j;

	int board [5][5];
	for (i=0; i<LENGTH; i++){
		for (j=0; j<WIDTH; j++){
			board[i][j] = 0;
		}
	}


	int path [25];


    /*
	for each square on the board find all tours
	*/

	

	for (i=0; i<LENGTH; i++){
		for (j=0; j<WIDTH; j++){
			path[0] = (i*5) + j;
			findTours(board, path, i, j, 1);
		}
	}


	return 0;
	
}

