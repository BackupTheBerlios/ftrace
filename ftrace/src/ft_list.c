/*
** ft_list.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 23:35:28 2006 
** Last update Wed Jan 18 02:40:40 2006 
*/



#include <stdlib.h>
#include <ft_list.h>


/* Doubly linked list
 */


/* Internal */

static void __attribute__((unused)) link_node_after(ft_list_t** head, ft_list_t* node)
{
  if (!*head)
    {
      *head = node;
    }
  else
    {
      node->prev = *head;
      if ((*head)->next)
	{
	  node->next = (*head)->next;
	  (*head)->next->prev = node;
	}
      (*head)->next = node;
    }
}

static void link_node_before(ft_list_t** head, ft_list_t* node)
{
  if (!*head)
    {
      *head = node;
    }
  else
    {
      node->next = *head;
      if ((*head)->prev)
	{
	  node->prev = (*head)->prev;
	  (*head)->prev->next = node;
	}
      (*head)->prev = node;
    }
}

static void unlink_node(ft_list_t** head, ft_list_t* node)
{
  if (*head == node)
    {
      *head = node->next;
      if (node->next)
	{
	  (*head)->prev = 0;
	  node->next->prev = 0;
	}
    }
  else
    {
      if (node->next)
	node->next->prev = node->prev;
      node->prev->next = node->next;
    }
}

static ft_list_t* create_list(void* data)
{
  ft_list_t* list;

  list = (ft_list_t*)malloc(sizeof(ft_list_t));
  if (list)
    {
      list->next = 0;
      list->prev = 0;
      list->data = data;
    }
  return list;
}


/* Exported */

ft_list_t* ft_list_init(void)
{
  return 0;
}

void ft_list_release(ft_list_t** list, void (*relfn)(void*))
{
  ft_list_t* cur;
  ft_list_t* sav;

  cur = *list;
  while (cur)
    {
      sav = cur;
      cur = cur->next;
      if (relfn)
	relfn(sav->data);
      free(sav);
    }
  *list = 0;
}

void ft_list_foreach(ft_list_t* list, void (*applyfn)(void*, void*), void* aux)
{
  while (list)
    {
      applyfn(list->data, aux);
      list = list->next;
    }
}

ft_list_t* ft_list_lkp(ft_list_t* list, int (*cmpfn)(void*, void*), void* data)
{
  while (list)
    {
      if (cmpfn(list->data, data) == 0)
	return list;
      list = list->next;
    }
  return 0;
}

int ft_list_push_front(ft_list_t** list, void* aux)
{
  ft_list_t* node;

  node = create_list(aux);
  if (node)
    {
      link_node_before(list, node);
      *list = node;
      return 0;
    }
  return -1;
}

int ft_list_push_back(ft_list_t** list, void* aux)
{
  ft_list_t* node;
  ft_list_t* ptr;

  node = create_list(aux);
  if (node == 0)
    return -1;

  if (*list == 0)
    {
      *list = node;
    }
  else
    {
      for (ptr = *list; ptr->next; ptr = ptr->next)
	;
      node->prev = ptr;
      ptr->next = node;
    }

  return 0;
}

void ft_list_remove(ft_list_t** head, ft_list_t* node, void (*relfn)(void*))
{
  unlink_node(head, node);
  if (relfn)
    relfn(node->data);
  free(node);
}
