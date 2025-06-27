#include <stdio.h>

int main() {


	char a[]= "helloeveryone";
	char b[]= "hiiii";
	memcpy(b,a+1,4);
	printf("%s",b);

return 0;
}
