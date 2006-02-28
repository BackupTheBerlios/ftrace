/*
** ft_frame.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sun Jan 15 20:51:54 2006 
** Last update Thu Jan 19 11:30:38 2006 fabien le-mentec
*/


#include <ft_proc.h>
#include <ft_frame.h>
#include <ft_error.h>
#include <ft_symbol.h>


/* Allocate and reset a new frame */
static ft_frame_t* allocate_new_frame(void)
{
  ft_frame_t* fr;

  fr = malloc(sizeof(ft_frame_t));
  if (fr)
    {
      fr->sp = 0;
      fr->bp = 0;
      fr->ret_pc = 0;
      fr->callee = 0;
      fr->depth = 0;
    }
  return fr;
}


ft_error_t ft_frame_get_current(ft_frame_t** frame, struct ft_proc* proc)
{
  ft_frame_t* fr;

  /* Allocate a new frame */
  fr = allocate_new_frame();
  *frame = 0;
  fr = allocate_new_frame();
  if (fr == 0)
    return FT_ERR_MALLOC;

  /* Get the current frame */
  ft_frame_hwget_current(proc, fr);

  /* Affect */
  *frame = fr;

  return FT_ERR_SUCCESS;
}

ft_error_t ft_frame_get_previous(ft_frame_t** previous, ft_frame_t* current, struct ft_proc* proc)
{
  ft_frame_t* frame;

  /* Allocate a new frame */
  *previous = 0;
  frame = allocate_new_frame();
  if (frame == 0)
    return FT_ERR_MALLOC;

  /* Get frame, from hardware dependent routine */
  if (ft_frame_hwget_previous(proc, current, frame) == FT_ERR_BADFRAME)
    {
      *previous = 0;
      ft_frame_release(frame);
      return FT_ERR_BADFRAME;
    }

  *previous = frame;
  return FT_ERR_SUCCESS;
}

ft_error_t ft_frame_dissect(ft_frame_t* frame, ft_proc_t* proc)
{
  ft_error_t err;
  
  /* Tag for not yet got frame */
  if (frame == 0)
    return FT_ERR_SUCCESS;

  err = ft_frame_hwdissect(proc, frame);
  if (err != FT_ERR_SUCCESS)
    return err;

  return FT_ERR_SUCCESS;
}

ft_error_t ft_frame_release(ft_frame_t* frame)
{
  free(frame);
  return FT_ERR_SUCCESS;
}

char* ft_frame_dump(ft_frame_t* frame, ft_symbol_t* sym)
{
  static char buf[512];
  char indent[64];
  char* ptr;
  int sz;

  /* Indent the text */
  *indent = 0;
  for (sz = 0; sz < frame->depth; ++sz)
    strcat(indent, "   ");

  ptr = buf;
  sz = 0;
  if (sym)
    sz += snprintf(buf, sizeof(buf) - sz, "%s<%s>\n", indent, sym->ident);
  else
    sz += snprintf(buf, sizeof(buf) - sz, "%s<@0x%08lx>\n", indent, frame->callee);

  ptr = buf + sz;
  sz += snprintf(ptr, sizeof(buf) - sz,"%s{\n", indent);
  ptr = buf + sz;
  sz += snprintf(ptr, sizeof(buf) - sz,"%s   . bp     == @0x%08lx\n", indent, frame->bp);
  ptr = buf + sz;
  sz += snprintf(ptr, sizeof(buf) - sz,"%s   . sp     == @0x%08lx\n", indent, frame->sp);
  ptr = buf + sz;
  sz += snprintf(ptr, sizeof(buf) - sz,"%s   . ret_pc == @0x%08lx\n", indent, frame->ret_pc);
  ptr = buf + sz;
  sz += snprintf(ptr, sizeof(buf) - sz,"%s   . callee == @0x%08lx\n", indent, frame->callee);
  ptr = buf + sz;
  sz += snprintf(ptr, sizeof(buf) - sz,"%s};\n", indent);
  ptr = buf + sz;

  return buf;
}
