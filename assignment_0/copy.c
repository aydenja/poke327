#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]){
	char test[25] = "test";
	char b [25]; 
	test [0] = 'a';
	strcpy(b, test);
	printf("%s\n%s\n", b, test);
}




