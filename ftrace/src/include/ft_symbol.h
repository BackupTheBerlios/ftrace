/*
** ft_symbol.h for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Wed Jan 18 00:42:21 2006 
** Last update Wed Jan 18 18:18:49 2006 fabien le-mentec
*/


#ifndef FT_SYMBOL_H
# define FT_SYMBOL_H


#include <ft_basic_types.h>
#include <ft_error.h>


/* Forward declarations */
struct ft_proc;
struct ft_list;
struct ft_symfile;


/* file format */
typedef struct
{
  const char* name;
  ft_bool_t (*match_fn)(char* mmapping, unsigned int size);
  ft_error_t (*explore_fn)(struct ft_proc*, char*, unsigned int);
  ft_error_t (*getsyms_fn)(struct ft_list**, struct ft_symfile*);
} ft_symfmt_t;


/* Symbol defintion */
typedef struct ft_symbol
{
  /* Symbol identity */
  struct ft_symfile* symfile;
  char* ident;

  /* Object symbolized */
  ft_addr_t addr;
  ft_size_t size;

  /* Is the address resolved */
  ft_bool_t resolved;

  /* Format specific datas */
  void* pdata;

  /* methods */
  ft_error_t (*release_fn)(struct ft_proc*, struct ft_symbol*);
  ft_addr_t (*resolv_fn)(struct ft_proc*, struct ft_symbol*);
  char* (*dump_fn)(struct ft_symbol*);

} ft_symbol_t;


/* Symbol file descriptor */
typedef struct ft_symfile
{
  char* filename;
  struct ft_list* symlist;
  int fd;
  ft_addr_t base_addr;
  struct stat st;
  char* mmptr;
  size_t mmlen;
} ft_symfile_t;


/* Symbols related api */
ft_error_t ft_symbol_create(ft_symbol_t** sym, ft_symfile_t*, char* ident, ft_addr_t, ft_size_t);
ft_error_t ft_symbol_release(ft_symbol_t* sym);
ft_error_t ft_symbol_list_from_file(struct ft_list**, const char* filename);
ft_error_t ft_symbol_file_release(ft_symfile_t*);
ft_error_t ft_symbol_file_add(struct ft_proc*, const char* filename);
ft_error_t ft_symbol_query_byname(struct ft_proc*, const char* name, struct ft_symbol**);
ft_error_t ft_symbol_query_byaddr(struct ft_proc*, ft_addr_t addr, struct ft_symbol**);
int ft_symbol_match(ft_symbol_t* a, ft_symbol_t* b);
char* ft_symbol_dump(ft_symbol_t* sym);


/* Elf format related */
ft_bool_t elf_match(char*, unsigned int);
ft_error_t elf_explore(struct ft_proc*, char*, unsigned int);
ft_error_t elf_getsyms(struct ft_list**, ft_symfile_t*);


#endif /* ! FT_SYMBOL_H */
