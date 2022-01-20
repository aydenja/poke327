#include <stdio.h>
#include <stdlib.h>
#define LENGTH 5
#define WIDTH 5


/*
Defines the possible set of moves for a knight at a given space
*/
int xMove[8] = { 2, 1, -1, -2, -2, -1, 1, 2 };
int yMove[8] = { 1, 2, 2, 1, -1, -2, -2, -1 }; 


int possibleMove (int y, int x, int visited[5][5]){
	
	if((y >= 0 && y < LENGTH) && (x>=0 && x < WIDTH) ){
		if (visited[y][x] == 0){
			return 1;
		}
		else{
			printf("cant move to %d, %d visited, %dvalue\n", y,x, visited[y][x]);
			return 0;
		}
	}
	else{
		return 0;
	}
}

int hasMove (int posY, int posX, int visited[5][5]){
	int i;
	for(i=0; i<8; i++){
		if(possibleMove(posY + yMove[i], posX + xMove[i], visited) == 1){
			return 1;
		}
	}
	return 0;
}

void printOutput (int path [25]){
	int i;
	for (i=0; i<25; i++){
		printf("%d, ", path[i]);
	}
	printf("\n");
}

void copyBoard (int og [5][5], int cp [5][5]){
	int i;
	int j; 

	for (i=0; i<5; i++){
		for (j=0; i<5; j++){
			cp[i][j] = og [i][j];
		}
	}
}

void findTours (int  visited [5][5], int  path [25],
 int posY, int posX, int pLength) {
	
	printf("visited %d, %d\n", posY, posX);
	visited[posY][posX] = 1;

	if(hasMove(posY, posX, visited) == 0){
		printf("found\n");
		if (pLength == 25){
			printOutput(path);
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

			if(possibleMove(nextY, nextX, visited) == 1){
				int newBoard [5][5];
				copyBoard (visited, newBoard);
				path[pLength] = (nextY*5) + nextX;
				printf("%d\n", path[pLength]);
				findTours(visited, path, nextY, nextX, pLength+1);
				printf("back\n");
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
			findTours(board, path, i, j, 0);
		}
	}


	return 0;
	
}

