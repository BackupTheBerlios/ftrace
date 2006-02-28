/*
** ttrace.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 12:07:22 2006 
** Last update Tue Jan 24 20:18:55 2006 fabien le-mentec
*/


#include <sys/ttrace.h>
#include <sys/wait.h>
#include <machine/sys/uregs.h>
#include <ft_proc.h>
#include <ft_event.h>
#include <ft_error.h>


/* Convenient macro */
#define xttrace( request, thid, addr1, data, addr2 )				\
({										\
  int __val;									\
  uint64_t __addr1 = addr1;							\
  uint64_t __addr2 = addr2;							\
  uint64_t __data = data;							\
  if (proc->done == true)							\
     return FT_ERR_TRFAILED;                                                    \
  do										\
    {										\
      errno = 0;								\
      __val = ttrace(request, proc->pid, thid, __addr1, __data, __addr2);	\
    } while (__val == -1 && (errno == EINTR || errno == EAGAIN));		\
  if (__val == -1 && errno)							\
    {										\
      perror("ttrace");								\
      proc->done = true;							\
      return FT_ERR_TRFAILED;							\
    }										\
  __val;									\
})

#define xttrace_wait( thid, opt , st )					\
({									\
  int __val;								\
  if (proc->done == true)						\
    return FT_ERR_TRFAILED;						\
  do									\
    {									\
      errno = 0;							\
      __val = ttrace_wait(proc->pid, thid, opt, st, sizeof(ttstate_t));	\
    }									\
  while (__val == -1 && (errno == EINTR || errno == EAGAIN));		\
  if (__val == -1 && errno)						\
    {									\
      proc->done = true;						\
      return FT_ERR_TRFAILED;						\
    }									\
  __val;								\
})


/* Process api sw dependent part implementation
 */

ft_error_t ft_proc_init_swdep(ft_proc_t* proc)
{
  bzero((void*)&proc->swdep, sizeof(proc->swdep));
  proc->swdep.request = TT_LWP_SINGLE;
  proc->swdep.pc = TT_NOPC;
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
      if (src->address.regid == FT_REG_PC)
	xttrace(TT_LWP_RUREGS, proc->swdep.prevst.tts_lwpid, (uint64_t)__ip, sizeof(uint64_t), (uint64_t)(unsigned long)dst);
      else if (src->address.regid == FT_REG_BP)
	xttrace(TT_LWP_RUREGS, proc->swdep.prevst.tts_lwpid, (uint64_t)__ip, sizeof(uint64_t), (uint64_t)(unsigned long)dst);
      else if (src->address.regid == FT_REG_SP)
	xttrace(TT_LWP_RUREGS, proc->swdep.prevst.tts_lwpid, (uint64_t)__sp, sizeof(uint64_t), (uint64_t)(unsigned long)dst);
      else
	err = FT_ERR_NOTIMPL;
      break;
    case FT_LOCID_VADDR:
      xttrace(TT_PROC_RDDATA, 0, (uint64_t)src->address.vaddr, (uint64_t)sz, (uint64_t)dst);
      break;
    case FT_LOCID_STRING:
      xttrace(TT_PROC_RDDATA, 0, (uint64_t)src->address.vaddr, (uint64_t)sz, (uint64_t)dst);
      break;
    default:
      err = FT_ERR_NOTIMPL;
      break;
    }
  return err;
}

ft_error_t ft_proc_writemm(ft_proc_t* proc, ft_mmloc_t* dst, ft_size_t sz, ft_addr_t from)
{
  ft_error_t err;

  err = FT_ERR_SUCCESS;
  switch (dst->id)
    {
    case FT_LOCID_REG:
      err = FT_ERR_NOTIMPL;
      break;
    case FT_LOCID_VADDR:
      xttrace(TT_PROC_WRDATA, 0, (uint64_t)dst->address.vaddr, (uint64_t)sz, (uint64_t)from);
      break;
    case FT_LOCID_STRING:
      err = FT_ERR_NOTIMPL;
      break;
    default:
      err = FT_ERR_NOTIMPL;
      break;
    }
  return err;
}

ft_error_t ft_proc_traceme(ft_proc_t* proc)
{
  xttrace(TT_PROC_SETTRC, 0, 0, TT_VERSION, 0);
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_attach(ft_proc_t* proc)
{
  ttevent_t ev;
  ttstate_t st;
  siginfo_t si;

  /* Attach a non child process.
     After attaching, the process is
     in a stopped state.
   */
  if (proc->child == false)
    {
      xttrace(TT_PROC_ATTACH, 0, TT_DETACH_ON_EXIT, TT_VERSION, 0);
      if (waitid(P_PID, proc->pid, &si, WEXITED | WSTOPPED) < 0 || si.si_pid != proc->pid || si.si_code != CLD_STOPPED)
	{
	  proc->done = true;
	  return FT_ERR_TRFAILED;
	}
    }

  /* Set the events to wait for */
  ev.tte_events = TTEVT_BPT_SSTEP | TTEVT_EXIT;
  ev.tte_opts = TTEO_NONE;
  xttrace(TT_PROC_SET_EVENT_MASK, 0, (uint64_t)((unsigned long)&ev), sizeof(ttevent_t), 0);

  /* Tell the child we are ok, it can exec by now */
  if (proc->child == true && proc->ipcfd > 0)
    {
      close(proc->ipcfd);
      if (waitid(P_PID, proc->pid, &si, WEXITED | WSTOPPED) < 0 || si.si_pid != proc->pid || si.si_code != CLD_STOPPED)
	{
	  proc->done = true;
	  return FT_ERR_TRFAILED;
	}
    }

  /* Get the state of the stopped thread */
  xttrace(TT_PROC_GET_FIRST_LWP_STATE, 0, (uint64_t)((unsigned long)&st), (uint64_t)sizeof(st), 0);
  proc->swdep.prevst = st;

  /* Continue the stopped thread, single stepping mode */
  xttrace(TT_LWP_SINGLE, st.tts_lwpid, TT_NOPC, 0, 0);


  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_detach(ft_proc_t* proc)
{
  xttrace(TT_PROC_DETACH, 0, 0, 0, 0);
  return FT_ERR_SUCCESS;
}


/* ! Remember filling the event content */
ft_error_t ft_proc_wait_event(ft_proc_t* proc, ft_event_t* ev)
{
  uint64_t pc;
  ttstate_t st;
  int signo;
  uint64_t request;

  /* Wait for the process thread to stop */
  xttrace_wait(0, TTRACE_WAITOK, &st);

  /* Initialize */
  signo = 0;
  request = TT_LWP_SINGLE;
  pc = TT_NOPC;

  /* Guess the reason */
  if (st.tts_event & TTEVT_BPT_SSTEP)
    {
      request = TT_LWP_SINGLE;
      ev->id = FT_EVID_WATCHPOINT;
      ev->data.wpt = 0;
    }
  else if (st.tts_event & TTEVT_SIGNAL)
    {
      signo = st.tts_u.tts_signal.tts_signo;
      ev->id = FT_EVID_STOPPED;
      ev->data.signo = signo;
    }
  else if (st.tts_event & TTEVT_EXIT)
    {
      ev->id = FT_EVID_EXIT;
      ev->data.ptr = 0;
      proc->done = true;
      proc->procval = st.tts_u.tts_exit.tts_exitcode;
    }

  proc->swdep.prevst = st;
  proc->swdep.request = request;
  proc->swdep.pc = pc;
  proc->swdep.signo = signo;

  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_continue(ft_proc_t* proc, ft_event_t* ev)
{
  xttrace(proc->swdep.request, proc->swdep.prevst.tts_lwpid, proc->swdep.pc, proc->swdep.signo, 0);
  return FT_ERR_SUCCESS;
}
