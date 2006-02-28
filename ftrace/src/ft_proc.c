/*
** ft_proc.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 13:56:14 2006 
** Last update Tue Jan 24 19:55:37 2006 fabien le-mentec
*/


#include <ft_basic_types.h>
#include <ft_watchpoint.h>
#include <ft_list.h>
#include <ft_error.h>
#include <ft_proc.h>



/* Internal
 */

static void proc_reset(ft_proc_t* proc)
{
  proc->pid = 0;
  proc->child = false;
  proc->done = false;
  proc->procval = 0;
  proc->imagepath = 0;
  proc->ipcfd = -1;
  proc->symfiles = ft_list_init();
  proc->watchpoints = ft_list_init();
  proc->interactiv = false;
  ft_proc_reset_swdep(proc);
}

static ft_proc_t* proc_alloc(void)
{
  ft_proc_t* proc;

  proc = malloc(sizeof(ft_proc_t));
  if (proc)
    proc_reset(proc);
  return proc;
}

static inline void proc_dealloc(ft_proc_t* proc)
{
  ft_list_release(&proc->symfiles, (void(*)(void*))ft_symbol_file_release);
  ft_list_release(&proc->watchpoints, (void(*)(void*))ft_watchpoint_release);

  if (proc->ipcfd != -1)
    close(proc->ipcfd);

  if (proc->imagepath)
    free(proc->imagepath);

  free(proc);
}


/* Exported
 */

ft_error_t ft_proc_init_from_pid(ft_proc_t** proc, ft_pid_t pid)
{
  ft_proc_t* p;

  *proc = 0;
  
  /* hu ;) */
  if (pid == getpid())
    return FT_ERR_BADPROC;

  p = proc_alloc();
  p->pid = pid;
  ft_proc_init_swdep(p);
  *proc = p;
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_init_from_av(ft_proc_t** proc, char** av, char** env)
{
  ft_proc_t* p;
  int pfather[2];
  int pchild[2];
  char buf;

  *proc = 0;
  p = proc_alloc();
  p->child = true;

  pipe(pfather);
  pipe(pchild);

  p->pid = fork();
  if (p->pid == 0)
    {
      /* Close unecessary fds */
      close(pchild[1]);
      close(pfather[0]);

      /* Set tracing mode */
      ft_proc_traceme(p);

      /* Tell our father we can be traced */
      close(pfather[1]);

      /* Wait for the father */
      read(pchild[0], &buf, sizeof(char));
      close(pchild[0]);

      /* Execute */
      execve(av[0], av, env);
      perror("execve");
      exit(-1);
    }

  p->ipcfd = pchild[1];
  ft_proc_init_swdep(p);
  close(pchild[0]);
  close(pfather[1]);
  read(pfather[0], &buf, sizeof(buf));
  close(pfather[0]);

  *proc = p;
  return FT_ERR_SUCCESS;
}

ft_error_t ft_proc_release(ft_proc_t** proc)
{
  ft_proc_release_swdep(*proc);
  proc_dealloc(*proc);
  *proc = 0;
  return FT_ERR_SUCCESS;
}
