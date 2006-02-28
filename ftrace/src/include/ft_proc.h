/*
** ft_proc_api.h for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 12:50:14 2006 
** Last update Sat Jan 21 13:08:25 2006 fabien le-mentec
*/


#ifndef FT_PROC_API_H
# define FT_PROC_API_H


/* Process manipulation api
 */


#include <ft_basic_types.h>
#include <ft_watchpoint.h>
#include <ft_symbol.h>
#include <ft_frame.h>
#include <ft_list.h>

/* Forward declarations */
struct ft_event;


/* Memory location description
 */
typedef enum
{
  FT_LOCID_REG = 0,
  FT_LOCID_VADDR,
  FT_LOCID_STRING,
  FT_LOCID_LOCAL,
  FT_LOCID_PARAM,
  FT_LOCID_SYM
} ft_mmid_t;

typedef enum
  {
    FT_REG_PC = 0,
    FT_REG_BP,
    FT_REG_SP
  } ft_regid_t;

typedef struct
{
  ft_mmid_t id;
  union
  {
    unsigned long offs;
    ft_regid_t regid;
    ft_addr_t vaddr;
  } address;
} ft_mmloc_t;

/* Convenient functions */
ft_mmloc_t* ft_mmloc_vaddr(ft_addr_t);


/* Process description
 */
typedef struct ft_proc
{
  /* system related */
  ft_pid_t pid;
  ft_bool_t child;
  ft_bool_t done;
  ft_procval_t procval;

  /* Informations */
  char* imagepath;

  /* symbol files */
  ft_list_t* symfiles;

  /* Tell the child it can execute */
  int ipcfd;

  /* Watchpoint list */
  ft_list_t* watchpoints;

  /* User interaction related */
  ft_bool_t interactiv;
  ft_bool_t do_prompt;

  /* Software dependent part */
  ft_procswdep_t swdep;
} ft_proc_t;


/* Process api
 */

/* Generic part */
ft_error_t ft_proc_init_from_pid(ft_proc_t**, ft_pid_t);
ft_error_t ft_proc_init_from_av(ft_proc_t**, char** av, char** env);
ft_error_t ft_proc_release(ft_proc_t**);

/* Software dependent part */
ft_error_t ft_proc_init_swdep(ft_proc_t*);
ft_error_t ft_proc_release_swdep(ft_proc_t*);
ft_error_t ft_proc_reset_swdep(ft_proc_t*);
ft_error_t ft_proc_readmm(ft_proc_t*, ft_mmloc_t* src, ft_size_t sz, ft_addr_t dst);
ft_error_t ft_proc_writemm(ft_proc_t*, ft_mmloc_t* dst, ft_size_t sz, ft_addr_t from);
ft_error_t ft_proc_traceme(ft_proc_t*);
ft_error_t ft_proc_attach(ft_proc_t*);
ft_error_t ft_proc_detach(ft_proc_t*);
ft_error_t ft_proc_wait_event(ft_proc_t*, struct ft_event*);
ft_error_t ft_proc_continue(ft_proc_t*, struct ft_event*);

/* Hardware dependent part */
ft_error_t ft_watchpoint_hwput(ft_proc_t*, struct ft_watchpoint*);
ft_error_t ft_watchpoint_hwdel(ft_proc_t*, struct ft_watchpoint*);
ft_error_t ft_watchpoint_hwmatch(ft_proc_t*, ft_watchpoint_t*, ft_addr_t, ft_watchmask_t);
ft_error_t ft_frame_hwget_current(ft_proc_t*, ft_frame_t*);
ft_error_t ft_frame_hwget_previous(ft_proc_t*, ft_frame_t* current, ft_frame_t* previous);
ft_error_t ft_frame_hwdissect(ft_proc_t*, ft_frame_t*);
char* ft_frame_hwdump(ft_frame_t*);

#endif /* ! FT_PROC_API_H */
