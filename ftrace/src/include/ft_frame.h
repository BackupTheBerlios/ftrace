/*
** ft_frame.h for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sun Jan 15 20:45:34 2006 
** Last update Thu Jan 19 10:11:37 2006 fabien le-mentec
*/


#ifndef FT_FRAME_H
# define FT_FRAME_H


#include <ft_error.h>
#include <ft_basic_types.h>


/* Forward declarations */
struct ft_proc;
struct ft_symbol;

typedef struct
{
  ft_addr_t bp;
  ft_addr_t sp;
  ft_addr_t ret_pc;

  ft_addr_t callee;

  int depth;

  /* Here include some hw
     dependent data
  */
} ft_frame_t;


/* @see ft_proc.h for hardware dependent part */
ft_error_t ft_frame_get_current(ft_frame_t**, struct ft_proc*);
ft_error_t ft_frame_get_previous(ft_frame_t**, ft_frame_t*, struct ft_proc*);
ft_error_t ft_frame_dissect(ft_frame_t*, struct ft_proc*);
ft_error_t ft_frame_release(ft_frame_t*);
char* ft_frame_dump(ft_frame_t*, struct ft_symbol*);


#endif /* ! FT_FRAME_H */
