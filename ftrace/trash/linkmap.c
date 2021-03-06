/*
** linkmap.c for  in /goinfre/ftrace/trash
** 
** Made by fabien le-mentec
** Login   <le-men_f@epita.fr>
** 
** Started on  Thu Jan 19 12:45:15 2006 fabien le-mentec
** Last update Thu Jan 19 13:09:03 2006 fabien le-mentec
*/


typedef struct elfobj
{
  unsigned int a;
  unsigned int b;
  struct elfobj* next;
  char* name;
  int c;
  int d;
  unsigned long base_addr;
  unsigned long size;
} elfobj_t;


static void dumpmap(void)
{
  unsigned long* ebx = (unsigned long*)42;
  elfobj_t* lmap;

  asm("movl %%ebx, %0"
      :"=m"(ebx));

  printf("ebx is @0x%x\n", ebx);
  lmap = (elfobj_t*)ebx[1];
  while (lmap)
    {
      printf("+ %s: 0x%x, 0x%x\n", lmap->name, lmap->base_addr, lmap->size);
      lmap = lmap->next;
    }
}


int main(int ac, char** av)
{
  dumpmap();
  return 0;
}
