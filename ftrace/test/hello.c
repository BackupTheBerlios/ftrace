/*
** hello.c for  in /home/texane/texane/wip/ftrace/test
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 14:49:14 2006 
** Last update Sat Jan 14 17:26:29 2006 
*/


static void writestr(const char* s)
{
  int i;
  i = 0;
  while (s[i])++i;
  write(1, s, i);
}


int main(int ac, char** av)
{
  writestr("hello\n");
  return 0;
}
