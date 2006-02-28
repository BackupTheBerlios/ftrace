/*
** watchpoint_sparcV9.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 18:49:43 2006 
** Last update Sat Jan 21 00:00:06 2006 
*/


#include <ft_proc.h>
#include <ft_error.h>
#include <ft_watchpoint.h>
#include <ft_basic_types.h>


/*
  We will implement breakpoint
  with the ta 1 instruction, as
  defined by the abi processor
  supplement for sparcv9
*/

#define BKPT_INSN	"\x91\xd0\x20\x01"


ft_error_t ft_watchpoint_hwput(ft_proc_t* proc, struct ft_watchpoint* wpt)
{
  unsigned long rqst[5];
  prwatch_t w;

  w.pr_vaddr = wpt->getaddr_fn(proc, wpt);
  w.pr_size = sizeof(ft_addr_t);
  w.pr_wflags = WA_EXEC;
  rqst[0] = PCWATCH;
  memcpy((void*)&rqst[1], (const void*)&w, sizeof(prwatch_t));
  if (write(proc->swdep.fdctl, (void*)rqst, sizeof(unsigned long) + sizeof(prwatch_t)) == -1)
    perror("cannot write");

  return FT_ERR_SUCCESS;
}

ft_error_t ft_watchpoint_hwdel(ft_proc_t* proc, struct ft_watchpoint* wpt)
{
  unsigned long rqst[5];
  prwatch_t w;

  w.pr_vaddr = wpt->getaddr_fn(proc, wpt);
  w.pr_size = sizeof(ft_addr_t);
  w.pr_wflags = 0; /* this clear the watch area */
  rqst[0] = PCWATCH;
  memcpy((void*)&rqst[1], (const void*)&w, sizeof(prwatch_t));
  if (write(proc->swdep.fdctl, (void*)rqst, sizeof(unsigned long) + sizeof(prwatch_t)) == -1)
    perror("cannot write");

  return FT_ERR_SUCCESS;
}

ft_error_t ft_watchpoint_hwmatch(ft_proc_t* proc, ft_watchpoint_t* wpt, ft_addr_t addr, ft_watchmask_t wmask)
{
  if (wpt->getaddr_fn(proc, wpt) == addr)
    return FT_ERR_SUCCESS;
  return FT_ERR_NOMATCH;
}
