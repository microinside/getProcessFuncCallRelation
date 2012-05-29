#include <stdio.h>

void a(){}
void b(){a();}
void c(){b();}
void d(){c();}

int main()
{
	while(1)
	{
	//sleep(1);
	a();
	b();
	c();
	d();
	c();
	b();
	a();
	}
	return 0;
}
