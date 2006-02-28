/*
** watchpoint_ia64.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 18:49:34 2006 
** Last update Sun Jan 15 01:34:26 2006 
*/


#include <ft_proc.h>
#include <ft_error.h>
#include <ft_watchpoint.h>


ft_error_t ft_watchpoint_hwput(ft_proc_t* proc, struct ft_watchpoint* wpt)
{
  return FT_ERR_SUCCESS;
}

ft_error_t ft_watchpoint_hwdel(ft_proc_t* proc, struct ft_watchpoint* wpt)
{
  return FT_ERR_SUCCESS;
}

ft_error_t ft_watchpoint_hwmatch(ft_proc_t* proc, ft_watchpoint_t* wpt, ft_addr_t addr, ft_watchmask_t wmask)
{
  return FT_ERR_NOMATCH;
}
