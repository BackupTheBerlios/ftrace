/*
** ft_event.h for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 17:40:11 2006 
** Last update Sun Jan 15 01:11:04 2006 
*/


#ifndef FT_EVENT_H
# define FT_EVENT_H


#include <ft_swdep.h>
#include <ft_error.h>
#include <ft_proc.h>
#include <ft_watchpoint.h>


/* Event identifiers */
typedef enum
  {
    /* User command */
    FT_EVID_COMMAND = 0,
    /* Watchpoint */
    FT_EVID_WATCHPOINT,
    /* The process has been stopped(signaling), can be restarted */
    FT_EVID_STOPPED,
    /* The process has been terminated(signaling), cannot be restarted */
    FT_EVID_TERMINATED,
    /* The process has made a call to exit() */
    FT_EVID_EXIT,
    /* Unknown command */
    FT_EVID_UNKNOWN
  } ft_evid_t;


typedef struct ft_event
{
  ft_evid_t id;
  union
  {
    int signo;
    ft_watchpoint_t* wpt;
    void* ptr;
  } data;
} ft_event_t;


/* Event handling main loop */
ft_error_t ft_event_reset(ft_event_t*);
ft_error_t ft_event_dispatch(ft_proc_t*, ft_event_t*);

#endif /* ! FT_EVENT_H */
