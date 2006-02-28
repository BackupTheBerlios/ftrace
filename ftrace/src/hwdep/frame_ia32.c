/*
** frame_ia32.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Mon Jan 16 23:32:57 2006 
** Last update Thu Jan 19 10:00:49 2006 fabien le-mentec
*/


/* Implement frame handling on the ia32 architecture */


#include <ft_error.h>
#include <ft_proc.h>
#include <ft_frame.h>
#include <ft_basic_types.h>



/* Exported frame manipulation
 */

ft_error_t ft_frame_hwget_current(ft_proc_t* proc, ft_frame_t* frame)
{
  ft_mmloc_t mmloc;

  /* Get base pointer */
  mmloc.id = FT_LOCID_REG;
  mmloc.address.regid = FT_REG_BP;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&frame->bp);

  /* Get top pointer */
  mmloc.id = FT_LOCID_REG;
  mmloc.address.regid = FT_REG_SP;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&frame->sp);

  /* This address is inside the callee */
  mmloc.id = FT_LOCID_REG;
  mmloc.address.regid = FT_REG_PC;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&frame->callee);

  /* Get the return eip */
  mmloc.id = FT_LOCID_VADDR;
  mmloc.address.vaddr = frame->bp + 4;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&frame->ret_pc);

  return FT_ERR_SUCCESS;
}


ft_error_t ft_frame_hwget_previous(ft_proc_t* proc, ft_frame_t* current, ft_frame_t* previous)
{
  ft_mmloc_t mmloc;
  long offset;

  /* previous ebp is at *(%ebp), in normal cases */
  mmloc.id = FT_LOCID_VADDR;
  mmloc.address.vaddr = current->bp;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&previous->bp);

  /* previous stack pointer is stored @current_bp + 8 */
  previous->sp = current->bp + 8;

  /* Get the return eip */
  mmloc.id = FT_LOCID_VADDR;
  mmloc.address.vaddr = current->bp + 4;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&previous->ret_pc);

  /* Get the callee address: retaddr + *(retaddr - 4)
     Actually, the offset in a relativ call is relativ
     to the pc @next instruction (think about call 0).
   */
  mmloc.id = FT_LOCID_VADDR;
  mmloc.address.vaddr = previous->ret_pc - 4;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&offset);

  previous->callee = previous->ret_pc + offset;

  return FT_ERR_SUCCESS;
}


ft_error_t ft_frame_hwdissect(ft_proc_t* proc, ft_frame_t* frame)
{
  ft_mmloc_t mmloc;
  ft_addr_t current_sp;

  /* We have reached the end of the backtrace */
  if (frame->bp == 0)
    return FT_ERR_LASTFRAME;

  /* Get top pointer */
  mmloc.id = FT_LOCID_REG;
  mmloc.address.regid = FT_REG_SP;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&current_sp);

  /* In the common case (ie. expand down stack) */
  if (frame->sp < current_sp)
    return FT_ERR_BADFRAME;
  if (frame->bp < current_sp)
    return FT_ERR_BADFRAME;

  return FT_ERR_SUCCESS;
}


char* ft_frame_hwdump(ft_frame_t* frame)
{
  /* Dump the current frame and ret the buffer */
  return "<the frame here>";
}
