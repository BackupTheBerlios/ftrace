/*
** ft_algorithm.h for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sun Jan 15 20:36:54 2006 
** Last update Sun Jan 15 20:41:00 2006 
*/


#ifndef FT_ALGORITHM_H
# define FT_ALGORITHM_H


#include <ft_basic_types.h>
#include <ft_error.h>


/* forward declarations */
struct ft_proc;

/* Functions prototypes */
ft_error_t ft_bktrace(struct ft_proc*, ft_addr_t);


#endif /* ! FT_ALGORITHM_H */
