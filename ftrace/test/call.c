/*
** call.c for  in /goinfre/ftrace/test
** 
** Made by fabien le-mentec
** Login   <le-men_f@epita.fr>
** 
** Started on  Thu Jan 19 09:11:21 2006 fabien le-mentec
** Last update Thu Jan 19 10:46:30 2006 fabien le-mentec
*/


void toto(void);

void foo(void)
{toto();
}


void toto(void)
{}


int main(int ac, char** av)
{
  toto();
  return 0;
}
