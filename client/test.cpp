#include <iostream>
#include <stdlib.h>


int main()
{

	char *str = (char*)malloc(4096*sizeof(char));

	std::cout << sizeof(str) << std::endl;
		

	free(str);

}
