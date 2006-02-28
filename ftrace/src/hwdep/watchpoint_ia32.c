/*
** watchpoint_ia32.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 18:46:27 2006 
** Last update Fri Jan 20 23:42:42 2006 
*/


#include <ft_proc.h>
#include <ft_error.h>
#include <ft_watchpoint.h>


static inline int make_trapping_insn(int insn)
{
  ((unsigned char*)&insn)[0x00] = 0xcc;
  return insn;
}


ft_error_t ft_watchpoint_hwput(ft_proc_t* proc, struct ft_watchpoint* wpt)
{
  ft_mmloc_t mmloc;
  int insn;

  /* Get the instruction sequence */
  mmloc.id = FT_LOCID_VADDR;
  mmloc.address.vaddr = wpt->getaddr_fn(proc, wpt);

  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&wpt->hwdep.saved_insn);

  /* Make a trapping instruction */
  insn = make_trapping_insn(wpt->hwdep.saved_insn);
  ft_proc_writemm(proc, &mmloc, sizeof(int), (ft_addr_t)&insn);

  return FT_ERR_SUCCESS;
}

ft_error_t ft_watchpoint_hwdel(ft_proc_t* proc, struct ft_watchpoint* wpt)
{
  ft_mmloc_t mmloc;
  int insn;

  /* Restore the old instruction */
  mmloc.id = FT_LOCID_VADDR;
  mmloc.address.vaddr = wpt->low_addr;
  insn = wpt->hwdep.saved_insn;
  ft_proc_writemm(proc, &mmloc, sizeof(int), (ft_addr_t)&insn);

  /* Act as a fault */
  proc->swdep.tr_pcaddr = (caddr_t)mmloc.address.vaddr;

  return FT_ERR_SUCCESS;
}

ft_error_t ft_watchpoint_hwmatch(ft_proc_t* proc, ft_watchpoint_t* wpt, ft_addr_t addr, ft_watchmask_t wmask)
{
  /* Match the breakpoint with the pc - 1(the pc points to the next instruction) */

  if ((wpt->low_addr + 1) == addr)
    return FT_ERR_SUCCESS;
  
  return FT_ERR_NOMATCH;
}
