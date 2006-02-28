/*
** ft_event.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 18:20:02 2006 
** Last update Sat Jan 21 15:39:42 2006 fabien le-mentec
*/


#include <ft_basic_types.h>
#include <ft_algorithm.h>
#include <ft_error.h>
#include <ft_event.h>


ft_error_t ft_event_reset(ft_event_t* ev)
{
  ev->id = FT_EVID_UNKNOWN;
  ev->data.ptr = 0;
  return FT_ERR_SUCCESS;
}


ft_error_t ft_event_dispatch(ft_proc_t* proc, ft_event_t* ev)
{
  switch (ev->id)
    {
    case FT_EVID_COMMAND:
      break;

    case FT_EVID_WATCHPOINT:
      ft_bktrace(proc, ev->data.wpt->getaddr_fn(proc, ev->data.wpt));
      break;

    case FT_EVID_STOPPED:
      printf("stopped\n");
      break;

    case FT_EVID_TERMINATED:
      printf("the process has been stopped, --> %ld\n", proc->procval);
      break;

    case FT_EVID_EXIT:
      printf("the process has been exited\n");
      break;

    default:
      break;
    }

  return FT_ERR_SUCCESS;
}
