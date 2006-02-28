/*
** ft_hwdep.h for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 12:58:46 2006 
** Last update Sun Jan 15 11:29:55 2006 fabien le-mentec
*/


#ifndef FT_HWDEP_H
# define FT_HWDEP_H


#ifdef FT_ARCH_IA32

/* Watchpoint hw dep part */
typedef struct
{
  unsigned int saved_insn;
} ft_wpthwdep_t;

#endif /* FT_ARCH_IA32 */



#ifdef FT_ARCH_IA64

/* Watchpoint hw dep part */
typedef struct
{
  unsigned char fake;
} ft_wpthwdep_t;

#endif /* FT_ARCH_IA64 */



#ifdef FT_ARCH_SPARCV9

/* Watchpoint hw dep part */
typedef struct
{
  unsigned char fake;
} ft_wpthwdep_t;

#endif /* FT_ARCH_SPARCV9 */



#endif /* ! FT_HWDEP_H */
