/*
** depth.c for  in /home/texane/texane/wip/ftrace/test
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 23:09:23 2006 
** Last update Sun Jan 15 15:37:05 2006 fabien le-mentec
*/


#include <stdio.h>


void bbar(int a)
{
  printf("------->bbar\n");
  printf("in b bar\n");
  printf("<-------bbar\n");
}


void bar(int a)
{
  printf("---->bar\n");
  bbar(a);
  printf("<----bar\n");
}


void foo(int a)
{
  printf("-->foo\n");
  bar(a);
  printf("<--foo\n");
}


int main(int ac, char** av)
{
  printf(">main\n");
  foo(42);
  printf("<main\n");
  return 0;
}
