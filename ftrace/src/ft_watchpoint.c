/*
** ft_watchpoint.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 14 19:54:48 2006 
** Last update Sat Jan 21 12:44:11 2006 fabien le-mentec
*/


#include <ft_proc.h>
#include <ft_error.h>
#include <ft_symbol.h>
#include <ft_basic_types.h>
#include <ft_watchpoint.h>



/* Watchpointa address getting functions
 */

static ft_addr_t wpt_getaddr_from_addr(struct ft_proc* proc, struct ft_watchpoint* wpt)
{
  /* Simple one, return the address */  
  return wpt->low_addr;
}

static ft_addr_t wpt_getaddr_from_label(struct ft_proc* proc, struct ft_watchpoint* wpt)
{
  /* Symbolic one, return address of the symbol */

  ft_addr_t addr;
  ft_symbol_t* sym;

  if (!proc)
    return 0;

  addr = 0;

  if (wpt->low_addr != 0)
    {
      wpt->getaddr_fn = wpt_getaddr_from_addr;
      addr = wpt->low_addr;
    }
  else
    {
      if (ft_symbol_query_byname(proc, wpt->label, &sym) == FT_ERR_SUCCESS)
	{
	  addr = sym->resolv_fn(proc, sym);
	  wpt->low_addr = sym->addr;
	  wpt->getaddr_fn = wpt_getaddr_from_addr;
	}
    }
    
  return addr;
}


/* Create a new watchpoint instance
 */
static ft_error_t ft_watchpoint_create(ft_watchpoint_t** wpt,
				       const char* label,
				       ft_error_t (*handler_fn)(ft_proc_t*, ft_watchpoint_t*),
				       ft_bool_t persistent,
				       ft_watchmask_t wmask)
{
  ft_addr_t low_addr;
  ft_watchpoint_t* w;
  ft_addr_t (*getaddr_fn)(ft_proc_t*, ft_watchpoint_t*);

  /* Init */
  *wpt = 0;
  low_addr = 0;
  getaddr_fn = wpt_getaddr_from_label;

  /* It s a direct address watchpoint */
  if (!strncmp("0x", label, 2))
    {
      if (sscanf(label, "0x%lx", &low_addr) == 0)
	return FT_ERR_BADLABEL;
      getaddr_fn = wpt_getaddr_from_addr;
    }


  w = malloc(sizeof(ft_watchpoint_t));
  if (w == 0)
    return FT_ERR_MALLOC;

  w->label = strdup(label);
  w->low_addr = low_addr;
  w->up_addr = 0;
  w->handler_fn = handler_fn;
  w->getaddr_fn = getaddr_fn;
  w->init_mask = wmask;
  w->set_mask = FT_WATCH_NONE;
  w->inserted = false;

  *wpt = w;

  return FT_ERR_SUCCESS;
}


/* Add a watchpoint to the list
 */
ft_error_t ft_watchpoint_add(ft_proc_t* proc,
			     const char* label,
			     ft_error_t (*handler_fn)(ft_proc_t*, ft_watchpoint_t*),
			     ft_bool_t persistent,
			     ft_watchmask_t wmask)
{
  ft_error_t err;
  ft_watchpoint_t* wpt;

  err = ft_watchpoint_create(&wpt, label, handler_fn, persistent, wmask);
  if (err == FT_ERR_SUCCESS)
    ft_list_push_front(&proc->watchpoints, (void*)wpt);

  return err;
}


/* Remove a watchpoint from the list
 */
struct wpt_match
{
  ft_addr_t addr;
  ft_proc_t* proc;
};

static int match_byaddr_fn(ft_watchpoint_t* wpt, struct wpt_match* ref)
{
  if (ft_watchpoint_hwmatch(ref->proc, wpt, ref->addr, FT_WATCH_ALL) == FT_ERR_NOMATCH)
    return -1;
  return 0;
}

ft_error_t ft_watchpoint_remove(ft_proc_t* proc, ft_watchpoint_t* wpt)
{
  ft_list_t* node;
  struct wpt_match wptmatch;
  
  wptmatch.addr = wpt->getaddr_fn(proc, wpt);
  wptmatch.proc = proc;

  node = ft_list_lkp(proc->watchpoints, (int(*)(void*, void*))match_byaddr_fn, (void*)&wptmatch);
  if (node == 0)
    return FT_ERR_NOMATCH;

  ft_list_remove(&proc->watchpoints, node, (void(*)(void*))ft_watchpoint_release);
  return FT_ERR_SUCCESS;
}


/* Release the watchpoint instance
 */
ft_error_t ft_watchpoint_release(ft_watchpoint_t* wpt)
{
  free(wpt->label);
  free(wpt);
  return FT_ERR_SUCCESS;
}


/* Watchpoint lookup, by address
 */
ft_error_t ft_watchpoint_lkp_byaddr(struct ft_proc* proc, ft_watchpoint_t** wpt, ft_addr_t addr, ft_watchmask_t wmask)
{
  ft_list_t* node;
  ft_bool_t ret;
  struct wpt_match wptmatch;
  
  wptmatch.addr = addr;
  wptmatch.proc = proc;

  *wpt = 0;
  node = ft_list_lkp(proc->watchpoints, (int(*)(void*, void*))match_byaddr_fn, (void*)&wptmatch);
  if (node == 0)
    return FT_ERR_NOMATCH;

  *wpt = (ft_watchpoint_t*)node->data;
  ret = ft_watchmask_isset(wmask, (*wpt)->init_mask);
  if (ret == false)
    return FT_ERR_NOMATCH;
  return FT_ERR_SUCCESS;
}


/* Dump a watchpoint
 */
char* ft_watchpoint_dump(ft_watchpoint_t* wpt)
{
#define BUFLEN 256
  static char buffer[BUFLEN];

  snprintf(buffer, sizeof(buffer),
	   "[@0x%08lx - @0x%08lx] %c%c%c %c%c%c -> $unknown",
	   wpt->low_addr, wpt->up_addr,
	   ft_watchmask_isset(wpt->init_mask, FT_WATCH_READ) ? 'r' : '-',
	   ft_watchmask_isset(wpt->init_mask, FT_WATCH_WRITE) ? 'w' : '-',
	   ft_watchmask_isset(wpt->init_mask, FT_WATCH_EXEC) ? 'x' : '-',
	   ft_watchmask_isset(wpt->set_mask, FT_WATCH_READ) ? 'r' : '-',
	   ft_watchmask_isset(wpt->set_mask, FT_WATCH_WRITE) ? 'w' : '-',
	   ft_watchmask_isset(wpt->set_mask, FT_WATCH_EXEC) ? 'x' : '-'
	   );
  return buffer;
}
