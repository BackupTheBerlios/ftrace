/*
** pic.c for  in /goinfre/lemen/ftrace/test
** 
** Made by fabien le-mentec
** Login   <le-men_f@epita.fr>
** 
** Started on  Wed Jan 18 10:55:46 2006 fabien le-mentec
** Last update Wed Jan 18 10:58:13 2006 fabien le-mentec
*/



static void foo(void)
{
  int a;
  static int c;

  a = c;
}

/* int main(int ac, char**av) */
/* { */
/*   return 0; */
/* } */
