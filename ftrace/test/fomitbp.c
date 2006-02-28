/*
** fomitbp.c for  in /home/texane/texane/wip/ftrace/test
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Tue Jan 17 00:37:35 2006 
** Last update Tue Jan 17 00:38:17 2006 
*/


#include <stdio.h>


void bar(void)
{
  printf("in bar\n");
}


void foo(void)
{
  bar();
}


int main(int ac, char** av)
{
  foo();
  return 0;
}
