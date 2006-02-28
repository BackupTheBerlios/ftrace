/*
** procfs.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 12:07:39 2006 
** Last update Sat Jan 21 12:18:13 2006 fabien le-mentec
*/


#include <fcntl.h>
#include <unistd.h>
#include <ft_proc.h>
#include <ft_event.h>
#include <ft_error.h>


/* Internal routines
 */

#define NR_MAXRQST 5

static inline int send_to_ctl(int fdctl, long* buf, int nelem)
{
  int ret;

  errno = 0;
  ret = write(fdctl, (void*)buf, nelem * sizeof(long));
  if (ret == -1 || errno)
    {
      if (errno == ENOENT || errno == EBADF)
	return -1;
    }
  return ret;
}

static ft_error_t turn_events(ft_proc_t* proc, ft_bool_t on)
{
  long rqst[NR_MAXRQST];
  sigset_t sigset;

  /* Remove all signals hold by the process, and catch' em */
  bzero((void*)&rqst[1], 4 * sizeof(long));
  sigemptyset(&sigset);
  rqst[0] = PCSHOLD;
  memcpy((void*)&rqst[1], (const void*)&sigset, sizeof(sigset_t));
  send_to_ctl(proc->swdep.fdctl, rqst, NR_MAXRQST);

  /* Set the signal to get from the debugger */
  bzero((void*)&rqst[1], 4 * sizeof(long));
  rqst[0] = PCSTRACE;
  sigemptyset(&sigset);
  if (on == true)
    sigfillset(&sigset);
  memcpy((void*)&rqst[1], (const void*)&sigset, sizeof(sigset_t));
  send_to_ctl(proc->swdep.fdctl, rqst, NR_MAXRQST);

  /* Fault tracing */
  rqst[0] = PCSFAULT;
  bzero((void*)&rqst[1], 4 * sizeof(long));
  if (on == true)
    {
      fltset_t fltset;
      fltset.word[0] = FLTBPT;
      fltset.word[1] = FLTTRACE;
      fltset.word[2] = FLTWATCH;
      memcpy((void*)&rqst[1], (const void*)&fltset, sizeof(fltset));
    }
  send_to_ctl(proc->swdep.fdctl, rqst, 2);

  /* clear the hold signal mask */
  bzero((void*)&rqst[1], 4 * sizeof(long));
  rqst[0] = PCSHOLD;
  sigemptyset(&sigset);
  memcpy((void*)&rqst[1], (const void*)&sigset, sizeof(sigset_t));
  send_to_ctl(proc->swdep.fdctl, rqst, NR_MAXRQST);

  return FT_ERR_SUCCESS;
}

/* Dump the processor context */
/* @/sys/procfs_isa.h */
/* lwp->pr_regs */


/* @see PCVADDR to let the process resume execution @vaddr */

static void __attribute__((unused)) dump_frame(ft_proc_t* proc, ft_regs_t* regs)
{
#define FRAME_SIZE 16
  ft_mmloc_t frame;
  unsigned long buf[FRAME_SIZE];
  unsigned int n;

  if (regs[R_PC] == 0x10758 ||
      regs[R_PC] == 0x106d4)
    {
      printf("regs\n");
      printf("{\n");
      printf("  . pc  == @0x%08x\n", regs[R_PC]);
      printf("  . npc == @0x%08x\n", regs[R_nPC]);
      printf("  . wim == @0x%08x\n", regs[R_WIM]);
      printf("  . fp  == @0x%08x\n", regs[R_I6]);
      printf("  . sp  == @0x%08x\n", regs[R_O6]);
      printf("};\n");

      if (regs[R_I6] == 0x0)
	{
	  printf("invalid/end frame\n");
	  return ;
	}

      /* Here dump the previous frame */
      frame.id = FT_LOCID_VADDR;
      frame.address.vaddr = (ft_addr_t)regs[R_I6];
      ft_proc_readmm(proc, &frame, sizeof(buf), (ft_addr_t)buf);
      printf("0x%08x ----------- \n", regs[R_I6]);
      for (n = 0; n < FRAME_SIZE; ++n)
	printf("         %s[%02d] == $0x%08lx\n", n < 8 ? "%l" : "%i", n % 8, buf[n]);
      printf("0x%08x ----------- \n", regs[R_I6] + FRAME_SIZE * sizeof(unsigned long));
    }
  
}

static ft_error_t handle_stopped_lwp(ft_proc_t* proc, ft_event_t* ev)
{
  long rqst;
  int fdstatus;
  int prstatus;
  pstatus_t pst;
  lwpstatus_t* lwpst;
  ft_watchpoint_t* wpt;

  /* Reset the event */
  ft_event_reset(ev);

  /* Get the process status */
  fdstatus = open(proc->swdep.stbuf, O_RDONLY);
  if (fdstatus == -1)
    {
      /* The process exited, reap its status */
      ev->id = FT_EVID_TERMINATED;
      proc->done = true;
      proc->procval = 0;
      do { errno = 0; } while (waitpid(proc->pid, &prstatus, WNOHANG) == -1 && errno == EINTR);
      if (errno == 0)
	{
	  if (WIFEXITED(prstatus))
	    {
	      ev->id = FT_EVID_EXIT;
	      proc->procval = WEXITSTATUS(prstatus);
	    }
	  else if (WIFSIGNALED(prstatus))
	    {
	      ev->id = FT_EVID_TERMINATED;
	      ev->data.signo = WTERMSIG(prstatus);
	      proc->procval = WEXITSTATUS(prstatus);
	    }
	}
      else
	{
	  return FT_ERR_BADFILE;
	}
      return FT_ERR_SUCCESS;
    }

  /* Read the events that have been posted */
  while (read(fdstatus, &pst, sizeof(pstatus_t)) == sizeof(pstatus_t))
    {
      /* Status can be got from here */
      lwpst = &pst.pr_lwp;

      /* Handle the status */
      if ((lwpst->pr_flags & PR_STOPPED) && (lwpst->pr_flags & PR_ISTOP))
	{
	  if (lwpst->pr_why == PR_SIGNALLED)
	    {
	      if (lwpst->pr_what == SIGTRAP)
		{
		  rqst = PCCSIG;

		  /* Watchpoint handling
		   */
		  switch (lwpst->pr_info.si_code)
		    {
		    case TRAP_BRKPT:
		    case TRAP_TRACE:
		    case TRAP_RWATCH:
		    case TRAP_WWATCH:
		    case TRAP_XWATCH:
		      if (ft_watchpoint_lkp_byaddr(proc, &wpt, (ft_addr_t)lwpst->pr_info.si_pc, FT_WATCH_ALL) == FT_ERR_SUCCESS)
			{
			  ev->id = FT_EVID_WATCHPOINT;
			  ev->data.wpt = wpt;
			  ft_watchpoint_hwdel(proc, wpt);
			}
		      break;
		    default:
		      break;
		    }
		  send_to_ctl(proc->swdep.fdctl, &rqst, 1);
		}
	      else
		{
		  long sigrqst[2];
		  sigrqst[0] = PCSSIG;
		  sigrqst[1] = lwpst->pr_cursig;
		  send_to_ctl(proc->swdep.fdctl, sigrqst, 2);
		}
	    }
	  else if (lwpst->pr_why == PR_FAULTED)
	    {
	      rqst = PCCFAULT;
	      send_to_ctl(proc->swdep.fdctl, &rqst, 1);
	    }
	}
    }
  close(fdstatus);
  return true;
}


static ft_error_t pwait(ft_proc_t* proc)
{
  long rqst;

  proc->swdep.stopped = false;
  rqst = PCWSTOP;
  send_to_ctl(proc->swdep.fdctl, &rqst, 1);
  proc->swdep.stopped = true;

  return true;
}

static ft_error_t pstep(ft_proc_t* proc)
{
  long rqst[2];

  rqst[0] = PCRUN;
  rqst[1] = PRSTEP | PRCFAULT;
  send_to_ctl(proc->swdep.fdctl, rqst, 2);
  return FT_ERR_SUCCESS;
}

static ft_error_t pattach(ft_proc_t* proc)
{
  char buf[64];
  long rqst[NR_MAXRQST];
  ft_event_t ev;

  proc->swdep.stopped = false;
  sprintf(buf, "/proc/%d/ctl", (int)proc->pid);
  proc->swdep.fdctl = open(buf, O_WRONLY | O_EXCL | O_NONBLOCK);
  if (proc->swdep.fdctl == -1)
    {
      proc->done = true;
      return FT_ERR_BADFILE;
    }

  sprintf(proc->swdep.stbuf, "/proc/%d/status", (int)proc->pid);
  sprintf(proc->swdep.asbuf, "/proc/%d/as", (int)proc->pid);

  /* Set events to look for */
  if (proc->child == false)
    {
      /* Non child process */
      rqst[0] = PCSTOP;
      send_to_ctl(proc->swdep.fdctl, rqst, 1);
      turn_events(proc, true);
      pstep(proc);
    }
  else
    {
      /* Child process has enabled tracing */
      printf("father: can go\n");
      close(proc->ipcfd);
      proc->ipcfd = -1;
      /* Waiting for it to stop */
      rqst[0] = PCWSTOP;
      send_to_ctl(proc->swdep.fdctl, rqst, 1);
      turn_events(proc, true);
      handle_stopped_lwp(proc, &ev);
      pstep(proc);
    }

  return FT_ERR_SUCCESS;
}

static ft_error_t pdetach(ft_proc_t* proc)
{
  long rqst[NR_MAXRQST];

  if (proc->done == true)
    return false;

  if (proc->swdep.stopped == false)
    pwait(proc);

  if (proc->swdep.fdctl != -1)
    {
      turn_events(proc, false);
      rqst[0] = PCRUN;
      rqst[1] = PRCFAULT;
      send_to_ctl(proc->swdep.fdctl, rqst, 2);
      close(proc->swdep.fdctl);
      proc->swdep.fdctl = -1;
      return true;
    }
  return FT_ERR_SUCCESS;
}



/* Process api sw dependent part implementation
 */

ft_error_t ft_proc_init_swdep(ft_proc_t* proc)
{
  proc->swdep.fdctl = -1;
  proc->swdep.stopped = false;
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_reset_swdep(ft_proc_t* proc)
{
  proc->swdep.stopped = false;
  proc->swdep.fdctl = -1;
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_release_swdep(ft_proc_t* proc)
{
  if (proc->swdep.fdctl != -1)
    close(proc->swdep.fdctl);
  ft_proc_reset_swdep(proc);
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_readmm(ft_proc_t* proc, ft_mmloc_t* src, ft_size_t sz, ft_addr_t dst)
{
  ft_error_t err;
  pstatus_t status;
  int fdstatus;
  int fd;

  fd = open(proc->swdep.asbuf, O_RDONLY);
  if (fd == -1)
    return FT_ERR_READ;

  err = FT_ERR_SUCCESS;

  switch (src->id)
    {

    case FT_LOCID_VADDR:
    case FT_LOCID_STRING:
      lseek(fd, (off_t)src->address.vaddr, SEEK_SET);
      if (read(fd, (void*)dst, sz) != sz)
	err = FT_ERR_READ;
      break;

    case FT_LOCID_REG:
      fdstatus = open(proc->swdep.stbuf, O_RDONLY);
      if (fdstatus == -1)
	{
	  perror("cannot open");
	  return FT_ERR_BADFILE;
	}
      if (read(fdstatus, &status, sizeof(status)) == -1)
	{
	  close(fdstatus);
	  perror("cannot read");
	  return FT_ERR_READ;
	}
      if (src->address.regid == FT_REG_PC)
	{
	  *(ft_addr_t*)dst = (ft_addr_t)status.pr_lwp.pr_reg[R_PC];
	}
      else if (src->address.regid == FT_REG_BP)
	{
	  *(ft_addr_t*)dst = (ft_addr_t)status.pr_lwp.pr_reg[R_I6];
	}
      else if (src->address.regid == FT_REG_SP)
	{
	  *(ft_addr_t*)dst = (ft_addr_t)status.pr_lwp.pr_reg[R_O6];
	}
      close(fdstatus);
      break;

    default:
      break;
    }

  close(fd);

  return err;
}

ft_error_t ft_proc_writemm(ft_proc_t* proc, ft_mmloc_t* dst, ft_size_t sz, ft_addr_t from)
{
  int fd;
  ft_error_t err;

  fd = open(proc->swdep.asbuf, O_WRONLY);
  if (fd == -1)
    return FT_ERR_WRITE;

  err = FT_ERR_SUCCESS;

  switch (dst->id)
    {
    case FT_LOCID_VADDR:
      lseek(fd, (off_t)dst->address.vaddr, SEEK_SET);
      if (write(fd, (void*)from, sz) != sz)
	err = FT_ERR_WRITE;
      break;
    default:
      break;
    }

  close(fd);

  return err;
}

ft_error_t ft_proc_traceme(ft_proc_t* proc)
{
  /* Let the child be traceable */
  if (ptrace(0, 0, 0, 0) == -1)
    perror("ptrace");
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_attach(ft_proc_t* proc)
{
  pattach(proc);
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_detach(ft_proc_t* proc)
{
  pdetach(proc);
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_wait_event(ft_proc_t* proc, ft_event_t* ev)
{
  pwait(proc);
  handle_stopped_lwp(proc, ev);
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_continue(ft_proc_t* proc, ft_event_t* ev)
{
  proc->swdep.run_av[0] = PCRUN;
  proc->swdep.run_av[1] = 0;
  proc->swdep.run_ac = 2;
  send_to_ctl(proc->swdep.fdctl, proc->swdep.run_av, proc->swdep.run_ac);

  return FT_ERR_SUCCESS;
}
