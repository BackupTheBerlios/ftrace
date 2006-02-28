/*
** frame_ia64.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Mon Jan 16 23:40:31 2006 
** Last update Tue Jan 17 00:52:08 2006 
*/


#include <ft_proc.h>
#include <ft_frame.h>
#include <ft_error.h>

ft_error_t ft_frame_hwget_current(ft_proc_t* proc, ft_frame_t* frame)
{
  return FT_ERR_SUCCESS;
}

ft_error_t ft_frame_hwget_previous(ft_proc_t* proc, ft_frame_t* current, ft_frame_t* previous)
{
  return FT_ERR_SUCCESS;
}

ft_error_t ft_frame_hwdissect(ft_proc_t* proc, ft_frame_t* frame)
{
  return FT_ERR_SUCCESS;
}

char* ft_frame_hwdump(ft_frame_t* frame)
{
  return "<the_frame_here>";
}
