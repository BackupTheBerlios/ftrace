/*
** ft_bktrace.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sun Jan 15 20:34:10 2006 
** Last update Fri Jan 20 23:57:52 2006 
*/


#include <ft_list.h>
#include <ft_proc.h>
#include <ft_error.h>
#include <ft_frame.h>
#include <ft_symbol.h>
#include <ft_algorithm.h>
#include <ft_basic_types.h>


static void display_frame_fn(ft_frame_t* frame, ft_proc_t* proc)
{
  ft_symbol_t* sym;
  char* buf;

  ft_symbol_query_byaddr(proc, frame->callee, &sym);
  buf = ft_frame_dump(frame, sym);
  printf("%s\n", buf);
}


ft_error_t ft_bktrace(ft_proc_t* proc, ft_addr_t from)
{
  int depth;
  ft_error_t err;
  ft_frame_t* current;
  ft_frame_t* previous;
  ft_list_t* framelist;

  /* allocate the frame list */
  framelist = ft_list_init();

  /* Bootstrap the algorithm */
  depth = 0;
  ft_frame_get_current(&current, proc);
  ft_list_push_front(&framelist, current);

  while ((err = ft_frame_dissect(current, proc)) == FT_ERR_SUCCESS)
    {
      ft_frame_get_previous(&previous, current, proc);
      previous->depth = ++depth;
      ft_list_push_front(&framelist, previous);
      current = previous;
    }

  /* An error occured */
  if (err == FT_ERR_BADFRAME)
    {
      printf("invalid frame detected backtracing 0x%08lx\n", from);
      return FT_ERR_BADFRAME;
    }
  else
    {
      /* Display the frame list */
      ft_list_foreach(framelist, (void (*)(void*, void*))display_frame_fn, proc);
    }

  /* Release the frame list */
  ft_list_release(&framelist, (void (*)(void*))ft_frame_release);

  return err;
}
