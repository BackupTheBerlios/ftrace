/*
** frame_sparcv9.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Mon Jan 16 23:41:49 2006 
** Last update Thu Jan 19 12:34:02 2006 fabien le-mentec
*/


#include <ft_proc.h>
#include <ft_frame.h>
#include <ft_error.h>
#include <ft_symbol.h>



/* Internal routines
 */

static inline ft_addr_t retpc_from_i7(ft_addr_t i7)
{
  /* Skip the instruction and the delay slot */
  return i7 + 8;
}


/* Exported
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

  /* Get the callee, on the form callee + offset */
  mmloc.id = FT_LOCID_REG;
  mmloc.address.regid = FT_REG_PC;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&frame->callee);

  /* Get the return eip */
  mmloc.id = FT_LOCID_VADDR;
  mmloc.address.vaddr = frame->sp + 15 * 4;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&frame->ret_pc);
  frame->ret_pc = retpc_from_i7(frame->ret_pc);

  return FT_ERR_SUCCESS;
}

ft_error_t ft_frame_hwget_previous(ft_proc_t* proc, ft_frame_t* current, ft_frame_t* previous)
{
  /* On the sparc architecture, the %sp/%o6 register
     has to point into a valid stack area in order
     for the window register overflow handler to
     save the current frame context.
     This state has to be at any time, because
     a signal can arise at any time.
     The stack frame layout is defined in the sparc
     processor abi supplement.
  */

  ft_symbol_t* sym;
  ft_mmloc_t mmloc;
  ft_error_t err;
  unsigned int insn;
  unsigned char prim_opcode;
  unsigned char sec_opcode;
  unsigned long disp;

  err = FT_ERR_SUCCESS;

  /* Get the previous sp */
  previous->sp = current->bp;

  /* Get the previous bp, @previous->sp + */
  mmloc.id = FT_LOCID_VADDR;
  mmloc.address.vaddr = previous->sp + 14 * 4;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&previous->bp);

  /* Get the return address, @previous->sp + */
  mmloc.id = FT_LOCID_VADDR;
  mmloc.address.vaddr = previous->sp + 15 * 4;
  ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&previous->ret_pc);
  previous->ret_pc = retpc_from_i7(previous->ret_pc);

  /* Get the callee address by disassembling the call instruction */
  if (previous->bp != 0x0)
    {
      mmloc.id = FT_LOCID_VADDR;
      mmloc.address.vaddr = previous->ret_pc - 8;
      ft_proc_readmm(proc, &mmloc, sizeof(ft_addr_t), (ft_addr_t)&insn);

      /* Decode the instruction */
      prim_opcode = (insn >> 30);
      if (prim_opcode == 0x0)
	{
	  sec_opcode  = (insn >> 21) & 0x7;
	  err = FT_ERR_BADINSN;
	}
      else if (prim_opcode == 0x1)
	{
	  disp = insn & ~(1 << 30);
	  disp = disp << 2;
	  previous->callee = previous->ret_pc - 8 + disp;
	}
      else if (prim_opcode == 0x3)
	{
	  err = FT_ERR_BADINSN;
	  disp = 0;
	}
      else
	{
	  err = FT_ERR_BADINSN;
	}

    }
  else
    {
      /* Here is a little hack:
	 _start is the first function called by the system.
	 at that time, nobody but the system called _start,
	 so the instruction is not available.
	 We force the callee to be _start.
      */
      previous->callee = 0;
      previous->ret_pc = 0;
      if (ft_symbol_query_byname(proc, "_start", &sym) == FT_ERR_SUCCESS)
	previous->callee = sym->addr;
    }

  
  return err;
}

ft_error_t ft_frame_hwdissect(ft_proc_t* proc, ft_frame_t* frame)
{
  ft_mmloc_t mmloc;
  ft_addr_t current_sp;

  /* According to _start, first instruction is clr %fp */
  if (frame->bp == 0x00000000)
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
  return "<the_frame_here>";
}
