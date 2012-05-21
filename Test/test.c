#include <stdio.h>
#include <unistd.h>

void
aaaa ()
{
}

void
bbbb ()
{
  printf ("go to sleep in b..\n");
  sleep (10);
  printf ("....\n");

  aaaa ();
}

void
cccc ()
{
  bbbb ();
}

void
dddd ()
{
  cccc ();
};

void
eeee ()
{
  printf ("go to sleep in e..\n");
  sleep (1);
  printf ("....\n");
  aaaa ();
// bbbb();
}

int
main ()
{
  dddd ();
  eeee ();
  return 0;
}
