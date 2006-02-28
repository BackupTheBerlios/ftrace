/*
** ft_watchpoint.h for  in /home/texane/texane/wip/ftrace/src/include
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 12:53:45 2006 
** Last update Fri Jan 20 23:22:19 2006 
*/


#ifndef FT_WATCHPOINT_H
# define FT_WATCHPOINT_H


/* 
   Watchpoints is the only abstraction
   used by our system; Breakpoints is
   an executable watchpoint.

   Watchpoints are what defines the
   tracer model; that's, writable watchpoint
   force us to single step the process and
   see wether a memory location has changed.

   Executable watchpoints are implemented
   by replacing a code instruction by a trapping
   one, then trying to match it in the trap handler.

   Watchpoints have to receive the tracer in parameter
   so that we can modify the behaviour according
   to the watchpoint type, and are able to instrument
   the process memory.
*/


#include <ft_basic_types.h>
#include <ft_hwdep.h>
#include <ft_error.h>


/* Forward declarations */
struct ft_proc;

typedef enum
  {
    FT_WATCH_NONE  = 0x0,
    FT_WATCH_READ  = 0x1,
    FT_WATCH_WRITE = 0x2,
    FT_WATCH_EXEC  = 0x4,
    FT_WATCH_ALL   = FT_WATCH_READ | FT_WATCH_WRITE | FT_WATCH_EXEC
  } ft_watchmask_t;

typedef struct ft_watchpoint
{
  /* Watchpoint state */
  ft_bool_t inserted;
  ft_bool_t persistent;
  ft_watchmask_t init_mask;
  ft_watchmask_t set_mask;

  /* Watched location */
  char* label;
  ft_addr_t low_addr;
  ft_addr_t up_addr;
/*   ft_addr_t location; */
/*   ft_symbol_t* */

  /* Watchpoint handler function */
  ft_error_t (*handler_fn)(struct ft_proc*, struct ft_watchpoint*);
  ft_addr_t (*getaddr_fn)(struct ft_proc*, struct ft_watchpoint*);

  /* Hardware dependent */
  ft_wpthwdep_t hwdep;

} ft_watchpoint_t;


/* set the bit */
#define ft_watchmask_set( mask, bit ) do { mask &= bit; } while (0)

/* clear the bit */
#define ft_watchmask_clr( mask, bit ) do { mask &= ~(bit); } while (0)

/* zero the whole mask */
#define ft_watchmask_zero( mask ) do { mask = FT_WATCH_NONE; } while (0)

/* is the bit set */
#define ft_watchmask_isset( mask, bit )		\
({						\
  ft_bool_t __set;				\
  __set = ((mask) & (bit)) ? true : false;	\
  __set;					\
})


/* Exported api */
ft_error_t ft_watchpoint_add(struct ft_proc*, const char* label, ft_error_t (*)(struct ft_proc*, struct ft_watchpoint*), ft_bool_t persistent, ft_watchmask_t wmask);
ft_error_t ft_watchpoint_remove(struct ft_proc*, ft_watchpoint_t*);
ft_error_t ft_watchpoint_lkp_byaddr(struct ft_proc*, ft_watchpoint_t**, ft_addr_t, ft_watchmask_t);
ft_error_t ft_watchpoint_release(ft_watchpoint_t*);
char* ft_watchpoint_dump(ft_watchpoint_t*);

/* @see ft_proc.h file for hw dependencies */

#endif /* ! FT_WATCHPOINT_H */
