/*
** no_syscall.c for  in /tmp/lemen/sources/ftrace/test
** 
** Made by fabien le-mentec
** Login   <le-men_f@epita.fr>
** 
** Started on  Tue Jan 24 13:54:13 2006 fabien le-mentec
** Last update Tue Jan 24 17:57:15 2006 fabien le-mentec
*/



void titi(void)
{
  printf("doing a call\n");
}


void toto(void)
{
  int n = 100;
  while (n >= 0)
    {
      titi();
      --n;
    }
}


int main(int ac, char** av)
{
  while (1)
    toto();
}
