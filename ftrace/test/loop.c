/*
** loop.c for  in /var/preserve/le-men_f/ft/ftrace/test
** 
** Made by fabien le-mentec
** Login   <le-men_f@epita.fr>
** 
** Started on  Sun Jan 15 12:23:37 2006 fabien le-mentec
** Last update Sat Jan 21 00:25:15 2006 
*/

#include <stdio.h>


void outstr(const char* s)
{
  int i;
  for (i = 0; s[i]; ++i);
  write(1, s, i);
}


int main(int ac, char** av)
{
  while (1) outstr("toto\n");
  return 0;
}
