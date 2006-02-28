/*
** ptrace.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 12:07:10 2006 
** Last update Tue Jan 24 18:23:10 2006 fabien le-mentec
*/


#include <ft_basic_types.h>
#include <ft_proc.h>
#include <ft_event.h>


#define xptrace( rqst, addr, data )			\
({							\
  int __ret;						\
  if (proc->done == true)				\
    __ret = -1;						\
  else							\
    {							\
      errno = 0;					\
      __ret = ptrace(rqst, proc->pid, addr, data);	\
      if (__ret == -1 && errno == ESRCH)		\
	proc->done = true;				\
    }							\
  __ret;						\
})


/* Internal functions, not exported
 */
static ft_error_t read_registers(ft_proc_t* proc, struct reg* regset)
{
  xptrace(PT_GETREGS, (caddr_t)regset, sizeof(struct reg));
  return FT_ERR_SUCCESS;
}

static ft_error_t read_mmblock(ft_proc_t* proc, unsigned long* to, ft_addr_t from, ft_size_t sz)
{
  ft_size_t n;

  for (n = 0; n < sz; n += sizeof(int))
    {
      *to = xptrace(PT_READ_D, (caddr_t)from + n, 0);
      ++to;
    }

  return FT_ERR_SUCCESS;
}

static ft_error_t read_mmstring(ft_proc_t* proc, unsigned long* to, ft_addr_t from, ft_size_t sz)
{
  return read_mmblock(proc, to, from, sz);
}

static ft_error_t write_mmblock(ft_proc_t* proc, ft_addr_t to, int* from, ft_size_t sz)
{
  ft_size_t n;

  for (n = 0; n < sz; n += sizeof(int))
    {
      xptrace(PT_WRITE_D, (caddr_t)to + n, *from);
      ++from;
    }

  return FT_ERR_SUCCESS;
}


/* Process api implementation, ptrace based
 */

ft_error_t ft_proc_init_swdep(ft_proc_t* proc)
{
  proc->swdep.tr_request = PT_CONTINUE;
  proc->swdep.tr_pcaddr = (caddr_t)1;
  proc->swdep.signo = 0;
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_release_swdep(ft_proc_t* proc)
{
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_reset_swdep(ft_proc_t* proc)
{
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_readmm(ft_proc_t* proc, ft_mmloc_t* src, ft_size_t sz, ft_addr_t dst)
{
  ft_error_t err;

  err = FT_ERR_SUCCESS;

  switch (src->id)
    {
    case FT_LOCID_REG:
      err = read_registers(proc, &proc->swdep.regset);
      if (src->address.regid == FT_REG_PC)
	*(ft_addr_t*)dst = proc->swdep.regset.r_eip;
      else if (src->address.regid == FT_REG_BP)
	*(ft_addr_t*)dst = proc->swdep.regset.r_ebp;
      else if (src->address.regid == FT_REG_SP)
	*(ft_addr_t*)dst = proc->swdep.regset.r_esp;
      break;
    case FT_LOCID_VADDR:
      err = read_mmblock(proc, (unsigned long*)dst, src->address.vaddr, sz);
    case FT_LOCID_STRING:
      err = read_mmstring(proc, (unsigned long*)dst, src->address.vaddr, sz);
    default:
      err = FT_ERR_NOTIMPL;
      break;
    }

  return err;
}

ft_error_t ft_proc_writemm(ft_proc_t* proc, ft_mmloc_t* dst, ft_size_t sz, ft_addr_t src)
{
  ft_error_t err;

  err = FT_ERR_SUCCESS;

  switch (dst->id)
    {
    case FT_LOCID_VADDR:
      err = write_mmblock(proc, (ft_addr_t)dst->address.vaddr, (int*)src, sz);
    default:
      err = FT_ERR_NOTIMPL;
      break;
    }

  return err;
}

ft_error_t ft_proc_traceme(ft_proc_t* proc)
{
  if (ptrace(PT_TRACE_ME, 0, 0, 0) == -1)
    proc->done = true;
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_attach(ft_proc_t* proc)
{
  /* here do your processing */
  if (proc->ipcfd > 0)
    close(proc->ipcfd);

  if (proc->child == false)
    xptrace(PT_ATTACH, 0, 0);

  waitpid(proc->pid, 0, 0);
  xptrace(PT_STEP, (caddr_t)1, 0);

  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_detach(ft_proc_t* proc)
{
  xptrace(PT_DETACH, 0, 0);
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_wait_event(ft_proc_t* proc, ft_event_t* ev)
{
  ft_error_t err;
  ft_watchpoint_t* wpt;
  int status;
  int ret;

  err = FT_ERR_SUCCESS;

  /* Reset event */
  ft_event_reset(ev);

  /* Wait for the process */
 try_again:
  errno = 0;
  ret = waitpid(proc->pid, &status, WALLSIG);
  if (ret == -1 && errno == EINTR)
    goto try_again;

  if (WIFEXITED(status))
    {
      proc->done = true;
      proc->procval = WEXITSTATUS(status);
      ev->id = FT_EVID_EXIT;
    }
  else if (WIFSIGNALED(status))
    {
      /* The process has been terminated by a signal */
      proc->done = true;
      proc->procval = WEXITSTATUS(status);
      ev->id = FT_EVID_TERMINATED;
      ev->data.signo = WTERMSIG(status);
    }
  else if (WIFSTOPPED(status))
    {
      ev->id = FT_EVID_STOPPED;
      ev->data.signo = WSTOPSIG(status);

      if (ev->data.signo == SIGTRAP)
	{
	  ft_mmloc_t loc_pc;
	  ft_addr_t pcval;

	  /* Don't redirect the signal */
	  ev->data.signo = 0;

	  /* Get the current instruction pointer */
	  loc_pc.id = FT_LOCID_REG;
	  loc_pc.address.regid = FT_REG_PC;
	  ft_proc_readmm(proc, &loc_pc, sizeof(unsigned long), (ft_addr_t)&pcval);

	  /* try to match a watchpoint */
	  if (ft_watchpoint_lkp_byaddr(proc, &wpt, pcval, FT_WATCH_ALL) == FT_ERR_SUCCESS)
	    {
	      ft_watchpoint_hwdel(proc, wpt);
	      ev->id = FT_EVID_WATCHPOINT;
	      ev->data.wpt = wpt;
	    }
	}
    }

  return err;
}

ft_error_t ft_proc_continue(ft_proc_t* proc, ft_event_t* ev)
{
  int signo;

  /* Get the signal to send */
  signo = 0;
  if (ev->id == FT_EVID_STOPPED)
    signo = ev->data.signo;

  xptrace(proc->swdep.tr_request, proc->swdep.tr_pcaddr, signo);
  return FT_ERR_SUCCESS;
}
