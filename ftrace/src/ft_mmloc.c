/*
** ft_mmloc.c for  in /home/texane/texane/school/ept3/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Fri Jan 20 22:40:52 2006 
** Last update Fri Jan 20 22:44:38 2006 
*/


/* Convenient functions to deal with memory locations
 */


#include <ft_proc.h>
#include <ft_basic_types.h>


ft_mmloc_t* ft_mmloc_vaddr(ft_addr_t vaddr)
{
  static ft_mmloc_t mmloc;

  mmloc.id = FT_LOCID_VADDR;
  mmloc.address.vaddr = vaddr;
  return &mmloc;
}
