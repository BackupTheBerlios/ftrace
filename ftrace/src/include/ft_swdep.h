/*
** ft_swdep.h for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 12:59:10 2006 
** Last update Tue Jan 24 11:35:40 2006 fabien le-mentec
*/


#ifndef FT_SWDEP_H
# define FT_SWDEP_H


/* Common to unices
 */
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>


/* Convenient types
 */
typedef enum { false=0, true }	ft_bool_t;
typedef pid_t			ft_pid_t;
typedef unsigned long		ft_addr_t;
typedef unsigned long		ft_size_t;
typedef unsigned long		ft_procval_t;


/* Os related part
 */
#ifdef __sun__
# include <elf.h>
# define MAP_FILE MAP_SHARED
# define DT_RPATH 15
# define DT_NEEDED 1
typedef struct
{
  Elf32_Word d_tag;
  union
  {
    Elf32_Word d_val;
    Elf32_Addr d_ptr;
  } d_un;
} Elf32_Dyn;
#endif /* __sun__ */



/* Process tracing api
 */

#ifdef FT_PROC_USE_PROCFS

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <elf.h>
#include <sys/fault.h>
#include <procfs.h>
#include <sys/user.h>
#include <sys/regset.h>
#define _STRUCTURED_PROC 1
#define _KERNEL 1
#include <sys/procfs.h>
#undef _KERNEL
#undef _STRUCTURED_PROC

typedef greg_t ft_regs_t;

typedef struct
{
  /* Control file */
  int fdctl;
  char stbuf[64];
  char asbuf[64];
  ft_bool_t stopped;

  /* Run request */
  long run_av[5];
  int run_ac;
} ft_procswdep_t;

#endif /* FT_PROC_USE_PROCFS */



#ifdef FT_PROC_USE_PTRACE

#include <sys/wait.h>
#include <sys/ptrace.h>
#include <machine/reg.h>

typedef struct
{
  struct reg regset;
  int tr_request;
  caddr_t tr_pcaddr;
  int signo;
} ft_procswdep_t;

#endif /* FT_PROC_USE_PTRACE */



#ifdef FT_PROC_USE_TTRACE

#include <sys/ttrace.h>

typedef struct
{
  int signo;
  uint64_t pc;
  uint64_t request;
  ttstate_t prevst;
} ft_procswdep_t;

#endif /* FT_PROC_USE_TTRACE */



#endif /* ! FT_SWDEP_H */
