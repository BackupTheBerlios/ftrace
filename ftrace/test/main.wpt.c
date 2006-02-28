/*
** main.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 12:04:33 2006 
** Last update Wed Jan 18 01:36:14 2006 
*/


#include <stdio.h>
#include <ft_proc.h>
#include <ft_event.h>
#include <ft_watchpoint.h>


extern char** environ;

static void wpt_insert_fn(void* data, void* aux)
{
  ft_watchpoint_t* wpt;

  wpt = (ft_watchpoint_t*)data;

  if (wpt->inserted == false)
    {
      ft_watchpoint_hwput((ft_proc_t*)aux, wpt);
      wpt->inserted = true;
    }
}

int main(int ac, char** av)
{
  ft_proc_t* proc;
  ft_event_t event;
  ft_addr_t waddr;

  {
    sscanf(av[1], "0x%lx", &waddr);
    --ac; ++av;
  }

  if (ac == 2)
    ft_proc_init_from_pid(&proc, atoi(av[1]));
  else
    ft_proc_init_from_av(&proc, av + 1, environ);

  ft_watchpoint_add(proc, waddr, 0, false, FT_WATCH_ALL);

  ft_proc_attach(proc);
  while (proc->done == false)
    {
      ft_proc_wait_event(proc, &event);
      ft_list_foreach(proc->watchpoints, wpt_insert_fn, (void*)proc);
      ft_event_dispatch(proc, &event);
      ft_proc_continue(proc, &event);
    }
  ft_proc_detach(proc);
  ft_proc_release(&proc);

  return 0;
}
