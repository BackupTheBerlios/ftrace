/*
** ft_cmd.h for  in /home/texane/texane/school/ept3/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 21 00:30:11 2006 
** Last update Sat Jan 21 10:58:05 2006 fabien le-mentec
*/


#ifndef FT_CMD_H
# define FT_CMD_H


#include <ft_list.h>
#include <ft_error.h>


/* Forward declarations */
struct ft_proc;

ft_error_t ft_cmd_interact_with_user(struct ft_proc**);
ft_error_t ft_cmd_list_from_av(ft_list_t** list, char** av);
ft_error_t ft_cmd_exec_list(ft_list_t*, struct ft_proc**);


#endif /* ! FT_CMD_H */
