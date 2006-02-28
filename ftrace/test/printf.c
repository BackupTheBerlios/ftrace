/*
** printf.c for  in /goinfre/lemen/ftrace/test
** 
** Made by fabien le-mentec
** Login   <le-men_f@epita.fr>
** 
** Started on  Sat Jan 21 15:23:41 2006 fabien le-mentec
** Last update Sat Jan 21 15:24:40 2006 fabien le-mentec
*/


void second(void)
{
  printf("ca roule dans second\n");
}


void first(void)
{
  printf("ca roule\n");
}


int main(int ac, char** av)
{
  first();
  second();
  return 0;
}
