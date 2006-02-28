/*
** ft_list.h<2> for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 23:36:25 2006 
** Last update Wed Jan 18 02:40:57 2006 
*/


#ifndef FT_LIST_H
# define FT_LIST_H


/* List data type */
typedef struct ft_list
{
  void*		data;
  struct ft_list* next;
  struct ft_list* prev;
} ft_list_t;


/* List manipulation */
ft_list_t*	ft_list_init(void);
void		ft_list_release(ft_list_t** list, void (*relfn)(void*));
void		ft_list_foreach(ft_list_t* list, void (*applyfn)(void*, void*), void* aux);
ft_list_t*	ft_list_lkp(ft_list_t* list, int (*cmpfn)(void*, void*), void* data);
int		ft_list_push_front(ft_list_t** list, void* aux);
int		ft_list_push_back(ft_list_t** list, void* aux);
void		ft_list_remove(ft_list_t** head, ft_list_t* node, void (*relfn)(void*));


#endif /* ! FT_LIST_H */
