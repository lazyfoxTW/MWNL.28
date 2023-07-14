#include <stdio.h>

int main() {

	int num = 1;

	printf("%d\n",num);

	num = num << 10;
	printf("%d\n",num);

	num = num >> 5;
	printf("%d\n",num);

	num = num >> 8;
	printf("%d\n",num);

	return 0;	
}