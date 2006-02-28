/*
** pc.c for  in /var/preserve/le-men_f/ft/ftrace/test
** 
** Made by fabien le-mentec
** Login   <le-men_f@epita.fr>
** 
** Started on  Sun Jan 15 12:44:13 2006 fabien le-mentec
** Last update Sun Jan 15 13:32:58 2006 fabien le-mentec
*/


#include <stdio.h>


extern unsigned long get_retpc(void);

int main(int ac, char** av)
{
  void (*ptrf)();

  printf("@pc == 0x%08lx\n", get_retpc());
  ptrf = (void(*)())(get_retpc());
  printf("second printf\n");
  ptrf();
  return 0;
}
