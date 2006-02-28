/*
** ft_symbol.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Wed Jan 18 00:42:13 2006 
** Last update Thu Jan 19 11:29:44 2006 fabien le-mentec
*/


#include <ft_list.h>
#include <ft_proc.h>
#include <ft_error.h>
#include <ft_symbol.h>
#include <ft_basic_types.h>


static ft_symfmt_t gbl_known_formats[] =
  {
    {"elf", elf_match, elf_explore, elf_getsyms},
    {0, 0, 0, 0}
  };


ft_error_t ft_symbol_create(ft_symbol_t** sym, ft_symfile_t* symfile, char* ident, ft_addr_t addr, ft_size_t size)
{
  ft_symbol_t* s;

  *sym = 0;

  /* Allocate the symbol */
  s = malloc(sizeof(ft_symbol_t));
  if (s == 0)
    return FT_ERR_MALLOC;
  
  /* Reset the symbol */
  s->symfile = symfile;
  s->ident = 0;
  s->addr = addr;
  s->size = size;
  s->resolved = false;

  /* Fill the symbol */
  if (ident)
    s->ident = strdup(ident);

  /* Affect */
  *sym = s;

  return FT_ERR_SUCCESS;
}


ft_error_t ft_symbol_release(ft_symbol_t* sym)
{
  if (sym->ident)
    free(sym->ident);
  free(sym);
  return FT_ERR_SUCCESS;
}


static ft_symfile_t* ft_symbol_file_init(const char* filename)
{
  ft_symfile_t* sf;

  sf = malloc(sizeof(ft_symfile_t));
  if (sf)
    {
      sf->base_addr = 0;
      sf->filename = strdup(filename);
      sf->symlist = ft_list_init();
      sf->fd = open(filename, O_RDONLY);
      if (sf->fd == -1)
	{
	  sf->mmptr = MAP_FAILED;
	  sf->mmlen = 0;
	}
      else
	{
	  /* Don't check here... */
	  fstat(sf->fd, &sf->st);
	  sf->mmlen = sf->st.st_size;
	  sf->mmptr = (char*)mmap(0, sf->mmlen, PROT_READ, MAP_FILE, sf->fd, 0);
	}
    }
  return sf;
}


ft_error_t ft_symbol_file_release(ft_symfile_t* sf)
{
  if (sf->filename)
    free(sf->filename);
  ft_list_release(&sf->symlist, (void(*)(void*))ft_symbol_release);
  if (sf->fd != -1)
    close(sf->fd);
  if (sf->mmptr != MAP_FAILED)
    munmap(sf->mmptr, sf->mmlen);
  free(sf);
  return FT_ERR_SUCCESS;
}


ft_error_t ft_symbol_list_from_file(struct ft_list** symfiles, const char* filename)
{
  ft_symfile_t* symfile;
  ft_symfile_t* current;
  ft_symfmt_t* symfmt;
  ft_list_t* node;

  /* Alloc the symfile descriptor */
  symfile = ft_symbol_file_init(filename);
  if (symfile == 0)
    return FT_ERR_MALLOC;

  /* Check if the file is an existing one */
  for (node = *symfiles; node; node = node->next)
    {
      current = (ft_symfile_t*)node->data;
      if (symfile->st.st_ino == current->st.st_ino &&
	  symfile->st.st_dev == current->st.st_dev)
	{
	  ft_symbol_file_release(symfile);
	  return FT_ERR_SUCCESS;
	}
    }
  
  /* Match a format */
  for (symfmt = &gbl_known_formats[0]; symfmt->match_fn && symfmt->match_fn(symfile->mmptr, symfile->mmlen) != true; ++symfmt)
    ;
  if (symfmt->match_fn)
    {
      ft_list_push_front(symfiles, (void*)symfile);
      symfmt->getsyms_fn(symfiles, symfile);

      /* Currently we don't keep those allocated */
      close(symfile->fd);
      symfile->fd = -1;
      munmap(symfile->mmptr, symfile->mmlen);
      symfile->mmptr = MAP_FAILED;
      symfile->mmlen = 0;
    }

  /* The format has not been matched */
  if (symfmt->match_fn == 0)
    {
      ft_symbol_file_release(symfile);
      return FT_ERR_BADFILE;
    }

  return FT_ERR_SUCCESS;
}

int ft_symbol_match(ft_symbol_t* a, ft_symbol_t* b)
{
  int ret;

  ret = 0;
  if (a->ident && b->ident)
    ret = strcmp(a->ident, b->ident);
  
  return ret;
}


char* ft_symbol_dump(ft_symbol_t* sym)
{
  static char buf[256];
  static char name[50];

  snprintf(name, sizeof(name), "%s::%s", sym->symfile->filename, sym->ident);

  snprintf(buf, sizeof(buf), "%s\t@0x%08lx [%3s]",
	   name, sym->addr, sym->resolved ? "abs" : "rel");
  return buf;
}


ft_error_t ft_symbol_file_add(struct ft_proc* proc, const char* filename)
{
  return ft_symbol_list_from_file(&proc->symfiles, filename);
}


ft_error_t ft_symbol_query_byname(struct ft_proc* proc, const char* name, ft_symbol_t** sym)
{
  ft_list_t* file_node;
  ft_list_t* sym_node;

  *sym = 0;
  for (file_node = proc->symfiles; file_node; file_node = file_node->next)
    {
      sym_node = ((ft_symfile_t*)file_node->data)->symlist;
      while (sym_node)
	{
	  if (!strcmp(name, ((ft_symbol_t*)sym_node->data)->ident))
	    {
	      *sym = (ft_symbol_t*)sym_node->data;
	      return FT_ERR_SUCCESS;
	    }
	  sym_node = sym_node->next;
	}
    }
  return FT_ERR_NOMATCH;
}


ft_error_t ft_symbol_query_byaddr(struct ft_proc* proc, ft_addr_t addr, ft_symbol_t** sym)
{
  /* This function tries to match
     the symbol the address belongs to.
     This is not an exact match, since
     we take into account symbol's size.
   */

  ft_addr_t symaddr;
  ft_symbol_t* current;
  ft_list_t* file_node;
  ft_list_t* sym_node;

  *sym = 0;
  for (file_node = proc->symfiles; file_node; file_node = file_node->next)
    {
      sym_node = ((ft_symfile_t*)file_node->data)->symlist;
      while (sym_node)
	{
	  current = (ft_symbol_t*)sym_node->data;
	  symaddr = current->resolv_fn(proc, current);
	  if (addr >= symaddr && addr < (symaddr + current->size))
	    {
	      *sym = current;
	      return FT_ERR_SUCCESS;
	    }
	  sym_node = sym_node->next;
	}
    }
  return FT_ERR_NOMATCH;
}
