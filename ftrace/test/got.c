/*
** got.c for  in /goinfre/lemen/ftrace/test
** 
** Made by fabien le-mentec
** Login   <le-men_f@epita.fr>
** 
** Started on  Sat Jan 21 13:41:57 2006 fabien le-mentec
** Last update Sat Jan 21 15:06:32 2006 fabien le-mentec
*/


int main(int ac, char** av)
{
  unsigned long got = 0;
  asm("mov %%ebx, %0":"=m"(got));
  printf("------> %x, %x\n",got ,got + 4);
  printf("got == %x\n", *(unsigned long*)(got + 4));
  return 0;
}
