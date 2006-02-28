/*
** ft_error.h for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 12:55:25 2006 
** Last update Tue Jan 24 11:06:40 2006 fabien le-mentec
*/


#ifndef FT_ERROR_H
# define FT_ERROR_H


typedef enum
  {
    FT_ERR_SUCCESS = 0,
    FT_ERR_MALLOC,
    FT_ERR_BADLABEL,
    FT_ERR_NOTIMPL,
    FT_ERR_NOMATCH,
    FT_ERR_BADSTATE,
    FT_ERR_BADPROC,
    FT_ERR_BADARGS,
    FT_ERR_TOOSMALL,
    FT_ERR_READ,
    FT_ERR_WRITE,
    FT_ERR_BADFILE,
    FT_ERR_BADFRAME,
    FT_ERR_LASTFRAME,
    FT_ERR_BADINSN,
    FT_ERR_TRFAILED,
  } ft_error_t;


char* ft_error_dump(ft_error_t);
char* ft_error_dump_with_param(ft_error_t, void*);


#endif /* ! FT_ERROR_H */
