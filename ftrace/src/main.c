/*
** main.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 12:04:33 2006 
** Last update Tue Jan 24 18:16:52 2006 fabien le-mentec
*/


#include <stdio.h>
#include <ft_cmd.h>
#include <ft_proc.h>
#include <ft_list.h>
#include <ft_event.h>
#include <ft_symbol.h>
#include <ft_watchpoint.h>


extern char** environ;


/* use this main to test the ia64/hp-ux host
 */
int main(int ac, char** av)
{
  ft_pid_t pid;
  ft_event_t event;
  ft_proc_t* proc;

  if (ac == 2)
    {
      pid = atoi(av[1]);
      ft_proc_init_from_pid(&proc, pid);
    }
  else
    {
      ft_proc_init_from_av(&proc, av + 1, environ);
    }

  ft_proc_attach(proc);
  while (proc->done == false)
    {
      ft_proc_wait_event(proc, &event);
      ft_proc_continue(proc, &event);
    }
  ft_proc_detach(proc);
  ft_proc_release(&proc);

  return 0;
}

/* static void wpt_insert_fn(void* data, void* aux) */
/* { */
/*   ft_watchpoint_t* wpt; */

/*   wpt = (ft_watchpoint_t*)data; */

/*   if (wpt->inserted == false) */
/*     { */
/*       ft_watchpoint_hwput((ft_proc_t*)aux, wpt); */
/*       wpt->inserted = true; */
/*     } */
/* } */

/* int main(int ac, char** av) */
/* { */
/*   ft_proc_t* proc; */
/*   ft_event_t event; */

/*   /\* Interaction before starting *\/ */
/*   proc = 0; */
/*   ft_cmd_interact_with_user(&proc); */
/*   if (proc == 0) */
/*     { */
/*       printf("no tracee, aborting\n"); */
/*       return -1; */
/*     } */

/*   ft_proc_attach(proc); */

/*   while (proc->done == false) */
/*     { */
/*       /\* Wait for an event to occur *\/ */
/*       ft_proc_wait_event(proc, &event); */

/*       /\* Dispatch the event *\/ */
/*       ft_event_dispatch(proc, &event); */

/*       /\* Handle user input *\/ */
/*       ft_cmd_interact_with_user(&proc); */

/*       /\* Reinsert watchpoints here *\/ */
/*       ft_list_foreach(proc->watchpoints, wpt_insert_fn, (void*)proc); */

/*       /\* Continue the process *\/ */
/*       ft_proc_continue(proc, &event); */
/*     } */
/*   ft_proc_detach(proc); */
/*   ft_proc_release(&proc); */

/*   return 0; */
/* } */
