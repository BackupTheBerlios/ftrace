/*
** elf.c for  in /home/texane/texane/wip/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Wed Jan 18 01:20:40 2006 
** Last update Tue Jan 24 10:55:38 2006 fabien le-mentec
*/


/* todos
   -> see the dependencies
   -> see the auxiliary vector
   -> man ld.so_elf
   !!! remove ft_list_init from ft_symbol.c
*/



#include <elf.h>
#include <ft_proc.h>
#include <ft_list.h>
#include <ft_error.h>
#include <ft_symbol.h>
#include <ft_swdep.h>


#ifdef __NetBSD__
/* Store by the dynamic linker */
/* @http://cvsweb.netbsd.org/bsdweb.cgi/src/libexec/ld.elf_so/rtld.h */
typedef struct rtldobj
{
  unsigned int a;
  unsigned int b;
  struct rtldobj* next;
  char* name;
  int c;
  int d;
  unsigned long base_addr;
  unsigned long size;
} rtldobj_t;

#endif /* __NetBSD__ */


/* Convenient macro, check the mmap boundaries
 */
#define CHECK_MMAP( base, len, ptr)				\
do								\
{								\
  unsigned long __ptr = (unsigned long)(ptr) + sizeof(*ptr); 	\
  unsigned long __mmap = (unsigned long)(base) + len; 		\
  if ( __ptr < ((unsigned long)base) ) return false; 		\
  if ( __ptr > __mmap ) return false;				\
}								\
while (0)


/* Resolution funtions
 */
ft_addr_t dont_resolve(ft_proc_t* proc, ft_symbol_t* sym)
{
  return sym->addr;
}

/* static ft_addr_t find_the_got(ft_proc_t* proc) */
/* { */
/*   #define BASE_ADDRESS (ft_addr_t)0x08048000 */

/*   ft_mmloc_t mmloc; */
/*   Elf32_Ehdr ehdr; */
/*   Elf32_Phdr* pharray; */
/*   Elf32_Dyn* dynarray; */
/*   ft_addr_t gotaddr; */
/*   unsigned int phnum; */
/*   unsigned int dynnum; */
/*   unsigned int n; */

/*   gotaddr = 0; */
/*   dynarray = 0; */
/*   pharray = 0; */

/*   mmloc.id = FT_LOCID_VADDR; */
/*   mmloc.address.vaddr = BASE_ADDRESS; */
/*   ft_proc_readmm(proc, &mmloc, sizeof(Elf32_Ehdr), (ft_addr_t)&ehdr); */
  
/*   phnum = ehdr.e_phnum; */
/*   pharray = malloc(phnum * sizeof(Elf32_Phdr)); */
/*   mmloc.id = FT_LOCID_VADDR; */
/*   mmloc.address.vaddr = BASE_ADDRESS + ehdr.e_phoff; */
/*   ft_proc_readmm(proc, &mmloc, phnum * sizeof(Elf32_Ehdr), (ft_addr_t)pharray); */
/*   for (n = 0; n < phnum; ++n) */
/*     { */
/*       if (pharray[n].p_type == PT_DYNAMIC) */
/* 	{ */
/* 	  dynnum = pharray[n].p_memsz / sizeof(Elf32_Dyn); */
/* 	  dynarray = malloc(dynnum * sizeof(Elf32_Dyn)); */
/* 	  mmloc.id = FT_LOCID_VADDR; */
/* 	  mmloc.address.vaddr = pharray[n].p_vaddr; */
/* 	  ft_proc_readmm(proc, &mmloc, dynnum * sizeof(Elf32_Dyn), (ft_addr_t)dynarray); */
/* 	  for (n = 0; n < dynnum; ++n) */
/* 	    { */
/* 	      if (dynarray[n].d_tag == DT_PLTGOT) */
/* 		{ */
/* 		  gotaddr = (ft_addr_t)dynarray[n].d_un.d_ptr; */
/* 		  n = dynnum; */
/* 		} */
/* 	    } */
/* 	  /\* End of search *\/ */
/* 	  n = phnum; */
/* 	} */
/*     } */

/*   if (pharray) */
/*     free(pharray); */
/*   if (dynarray) */
/*     free(dynarray); */

/*   return gotaddr; */
/* } */

/* static ft_addr_t lkp_linkmap(ft_proc_t* proc, struct stat* refst, ft_addr_t gotaddr) */
/* { */
/*   ft_mmloc_t mmloc; */
/*   rtldobj_t* pobj; */
/*   rtldobj_t obj; */
/*   char objname[512]; */
/*   struct stat st; */

/*   mmloc.id = FT_LOCID_VADDR; */
/*   mmloc.address.vaddr = gotaddr + 4; */
/*   ft_proc_readmm(proc, &mmloc, sizeof(pobj), (ft_addr_t)&pobj); */

/*   while (pobj) */
/*     { */
/*       /\* Read the object *\/ */
/*       mmloc.id = FT_LOCID_VADDR; */
/*       mmloc.address.vaddr = (ft_addr_t)pobj; */
/*       ft_proc_readmm(proc, &mmloc, sizeof(obj), (ft_addr_t)&obj); */

/*       /\* Read the string *\/ */
/*       mmloc.id = FT_LOCID_STRING; */
/*       mmloc.address.vaddr = (ft_addr_t)obj.name; */
/*       ft_proc_readmm(proc, &mmloc, sizeof(objname), (ft_addr_t)&objname); */

/*       /\* Compare by inode/dev numbers *\/ */
/*       if (stat(objname, &st) != -1) */
/* 	{ */
/* 	  if (st.st_ino == refst->st_ino && st.st_dev == refst->st_dev) */
/* 	    { */
/* 	      return obj.base_addr; */
/* 	    } */
/* 	} */

/*       /\* Next object *\/ */
/*       pobj = obj.next; */
/*     } */

/*   return 0; */
/* } */

/* ft_addr_t resolve_from_so_offset(ft_proc_t* proc, ft_symbol_t* sym) */
/* { */
/*   /\* In this function, resolve all unresolved symbols *\/ */
/*   ft_addr_t gotaddr; */
/*   ft_list_t* node; */

/*   if (sym->resolved == true) */
/*     return sym->addr; */

/*   /\* The base has not yet been resolved, do it now *\/ */
/*   if (sym->symfile->base_addr == 0) */
/*     { */
/*       gotaddr = find_the_got(proc); */
/*       sym->symfile->base_addr = lkp_linkmap(proc, &sym->symfile->st, gotaddr); */
/*       if (sym->symfile->base_addr == 0) */
/* 	return 0; */
      
/*       /\* Resolve the current symbol, avoiding infinite loop *\/ */
/*       sym->resolv_fn = dont_resolve; */
/*       sym->resolved = true; */
/*       sym->addr += sym->symfile->base_addr; */
      
/*       /\* Resolve all the symbols relativ to this object *\/ */
/*       for (node = sym->symfile->symlist; node; node = node->next) */
/* 	((ft_symbol_t*)node->data)->resolv_fn(proc, (ft_symbol_t*)node->data); */
/*     } */

/*   /\* Resolve the symbol if not yet done *\/ */
/*   if (sym->symfile->base_addr && sym->resolved == false) */
/*     { */
/*       sym->resolved = true; */
/*       sym->addr += sym->symfile->base_addr; */
/*     } */

/*   return sym->addr; */
/* } */



/* Maximum section number */
#define NR_MAX_SHNUM	512


/* Internal parsing routines
 */
typedef struct
{
  const char* name;
  const char* data;
  unsigned long size;
  unsigned long index;
  unsigned short type;
  unsigned short link;
  unsigned short info;
  unsigned short flags;
} elf_section_t;


static void section_release(void* section)
{
  free(section);
}

static void __attribute__((unused)) elf_section_dump(elf_section_t* section)
{
  printf("section[%lu]\n", section->index);
  printf("{\n");
  printf("  . name == %s\n", section->name ? section->name : "<?>");
  printf("  . data == @0x%p\n", section->data);
  printf("  . size == #0x%lx\n", section->size);
  printf("  . type == #0x%x\n", section->type);
  printf("  . link == #0x%x\n", section->link);
  printf("  . info == #0x%x\n", section->info);
  printf("  . info == #0x%x\n", section->flags);
  printf("};\n");
}

static void __attribute__((unused)) elf_section_dump_fn(void* section, void* aux)
{
  aux = 0;
  elf_section_dump((elf_section_t*)section);
}

static elf_section_t* elf_section_new(const char* name,
				      const char* data,
				      unsigned long size,
				      unsigned long index,
				      unsigned short type,
				      unsigned short link,
				      unsigned short info,
				      unsigned short flags)
{
  elf_section_t* section;

  section = malloc(sizeof(elf_section_t));
  if (section)
    {
      section->name = name;
      section->data = data;
      section->size = size;
      section->index = index;
      section->type = type;
      section->link = link;
      section->info = info;
      section->flags = flags;
    }
  return section;
}

static int elf_sections_to_list(const char* mmptr, size_t mmlen, ft_list_t** sections)
{
  Elf32_Ehdr* ehdr;
  Elf32_Shdr* shdr;
  elf_section_t* section;
  unsigned int nsection;
  unsigned int n;

  ehdr = (Elf32_Ehdr*)mmptr;
  CHECK_MMAP(mmptr, mmlen, ehdr);
  nsection = ehdr->e_shnum;

  if (nsection == 0 || nsection > NR_MAX_SHNUM)
    return -1;

  shdr = (Elf32_Shdr*)(mmptr + ehdr->e_shoff);
  for (n = 0; n < nsection; ++n)
    {
      CHECK_MMAP(mmptr, mmlen, &shdr[n]);
      CHECK_MMAP(mmptr, mmlen, &shdr[n].sh_offset);
      section = elf_section_new((const char*)shdr[n].sh_name,
				(const char*)(mmptr + shdr[n].sh_offset),
				shdr[n].sh_size, n, shdr[n].sh_type, shdr[n].sh_link,
				shdr[n].sh_info, shdr[n].sh_flags);
      ft_list_push_front(sections, (void*)section);
    }
  
  return 0;
}


static int match_byshndx_fn(void* section, void* data)
{
  return !(((elf_section_t*)section)->index == *(unsigned short*)data);
}

static void apply_section_name_fn(void* data, void* aux)
{
  elf_section_t* section;
  elf_section_t* strsection;

  section = (elf_section_t*)data;
  strsection = (elf_section_t*)aux;
  section->name = (char*)(strsection->data + (unsigned long)section->name);
}

static int elf_sections_get_informations(const char* mmptr, size_t mmlen, ft_list_t* sections)
{
  Elf32_Ehdr* ehdr;
  elf_section_t* strsection;
  ft_list_t* node;

  /* Get string section */
  ehdr = (Elf32_Ehdr*)mmptr;
  CHECK_MMAP(mmptr, mmlen, ehdr);
  node = ft_list_lkp(sections, match_byshndx_fn, (void*)&ehdr->e_shstrndx);
  if (strsection == 0)
    return -1;
  strsection = (elf_section_t*)node->data;

  /* Fill section names */
  ft_list_foreach(sections, apply_section_name_fn, (void*)strsection);
  
  return 0;
}


static int __attribute__((unused)) match_section_byname_fn(void* section, void* data)
{
  return strcmp(((elf_section_t*)section)->name, (char*)data);
}


static int __attribute__((unused)) match_section_bytype_fn(void* section, void* data)
{
  return !( ((elf_section_t*)section)->type == *(unsigned short*)data );
}

static int elf_get_exported_symbols(ft_symfile_t* symfile,
				    ft_list_t* sections,
				    ft_list_t** symbols)
{
  ft_bool_t sharedobject;
  Elf32_Sym* sym;
  elf_section_t* strsection;
  elf_section_t* symsection;
  elf_section_t* s;
  ft_list_t* node;
  ft_list_t* lnk;
  ft_symbol_t* cursym;
  unsigned int nsyms;
  unsigned int n;
  unsigned int symtype;
  char* mmptr;
  size_t mmlen;

  mmptr = symfile->mmptr;
  mmlen = symfile->mmlen;

  sharedobject = false;

  /* Is the file a shared library */
  {
    Elf32_Ehdr* ehdr;
    ehdr = (Elf32_Ehdr*)mmptr;
    if (ehdr->e_type == ET_DYN)
      sharedobject = true;
  }

  /* Get symbol table sections */
  for (node = sections; node; node = node->next)
    {
      s = (elf_section_t*)node->data;
      if (s->type == SHT_SYMTAB)
	{
	  lnk = ft_list_lkp(sections, match_byshndx_fn, (void*)&s->link);
	  strsection = (elf_section_t*)lnk->data;

	  /* find the section */
	  nsyms = s->size / sizeof(Elf32_Sym);
	  sym = (Elf32_Sym*)s->data;
	  CHECK_MMAP(mmptr, mmlen, sym);

	  for (n = 0; n < nsyms; ++n)
	    {
	      /* Get symbol type */
	      symtype = ELF32_ST_TYPE(sym[n].st_info);
	      switch (symtype)
		{
		case STT_FUNC:
		case STT_OBJECT:
		  lnk = ft_list_lkp(sections, match_byshndx_fn, (void*)&(sym[n].st_shndx));
		  if (lnk)
		    {
		      symsection = (elf_section_t*)lnk->data;
		      if (sym[n].st_name && (symsection->flags & SHF_EXECINSTR) && (ELF32_ST_BIND(sym[n].st_info) == STB_GLOBAL))
			{
			  ft_symbol_create(&cursym, symfile, (char*)(strsection->data + sym[n].st_name), (ft_addr_t)sym[n].st_value, (ft_size_t)sym[n].st_size);
			  if (ft_list_lkp(*symbols, (int (*)(void*, void*))ft_symbol_match, (void*)cursym) == 0)
			    {
			      ft_list_push_front(symbols, (void*)cursym);
			      if (sharedobject)
				{
				  /* cursym->resolv_fn = resolve_from_so_offset; */
				  cursym->resolv_fn = dont_resolve;
				  cursym->resolved = true;
				}
			      else
				{
				  cursym->resolv_fn = dont_resolve;
				  cursym->resolved = true;
				}
			    }
			  else
			    {
			      ft_symbol_release(cursym);
			    }
			}
		      break;

		    default:
		      break;
		    }
		}
	    }
	}
    }

  return 0;
}


static int elf_get_dependencies(const char* mmptr,
				size_t mmlen,
				ft_list_t* sections,
				ft_list_t** symbols,
				ft_list_t** symfiles)
{
  /* Dependencies can be found in 
     the DT_NEEDED entries of the
     dynamic section.
   */

  Elf32_Ehdr* ehdr;
  unsigned int ndyn;
  unsigned int maxdyn;
  Elf32_Dyn* pdyn;
  ft_list_t* node;
  ft_list_t* libpaths;
  ft_list_t* nodepath;
  elf_section_t* dynamic;
  elf_section_t* strtab;
  char sopath[256];

  /* Get the string section, this is subject to change */
  ehdr = (Elf32_Ehdr*)mmptr;
  CHECK_MMAP(mmptr, mmlen, ehdr);
  /*   node = ft_list_lkp(sections, match_section_bytype_fn, (void*)&sectype); */
  node = ft_list_lkp(sections, match_section_byname_fn, (void*)".dynstr");
  if (node == 0)
    return -1;
  strtab = (elf_section_t*)node->data;

  /* Bootstrap the path list */
  libpaths = ft_list_init();
  ft_list_push_front(&libpaths, (void*)"/usr/lib/");
  ft_list_push_front(&libpaths, (void*)"/lib/");
  ft_list_push_front(&libpaths, (void*)"/usr/local/lib/");

  /* Walk and find dynamic */
  for (node = sections; node; node = node->next)
    {
      dynamic = (elf_section_t*)node->data;
      if (dynamic->type == SHT_DYNAMIC)
	{
	  maxdyn = dynamic->size / sizeof(Elf32_Dyn);
	  pdyn = (Elf32_Dyn*)dynamic->data;

	  /* First pass to get the -rpath related data */
	  for (ndyn = 0; ndyn < maxdyn; ++ndyn)
	    {
	      if (pdyn[ndyn].d_tag == DT_RPATH)
		ft_list_push_front(&libpaths, (void*)(strtab->data + pdyn[ndyn].d_un.d_val));
	    }

	  /* Second pass to get library names */
	  for (ndyn = 0; ndyn < maxdyn; ++ndyn)
	    {
	      if (pdyn[ndyn].d_tag == DT_NEEDED)
		{
		  nodepath = libpaths;
		  while (nodepath)
		    {
		      snprintf(sopath, sizeof(sopath), "%s/%s", (char*)nodepath->data, strtab->data + pdyn[ndyn].d_un.d_val);
		      if (ft_symbol_list_from_file(symfiles, sopath) == FT_ERR_SUCCESS)
			nodepath = 0;
		      else
			nodepath = nodepath->next;
		    }
		}
	    }
	}
    }

  /* Release the path list */
  ft_list_release(&libpaths, 0);

  return 0;
}


/* Core file extraction related
 */

/* typedef struct netbsd_elfcore_procinfo elf_corinfo_t; */

/* int dissect_elf_core_file(const char* mmptr, size_t mmlen) */
/* { */
/*   /\* Core file is a serie of pht, each of one */
/*      describing memory areas of the dumped process. */
/*   *\/ */

/*   Elf32_Ehdr* ehdr; */
/*   Elf32_Phdr* phdr; */
/*   unsigned int nphdr; */
/*   unsigned int n; */

/*   ehdr = (Elf32_Ehdr*)mmptr; */
/*   CHECK_MMAP(mmptr, mmlen, ehdr); */
/*   phdr = (Elf32_Phdr*)(mmptr + ehdr->e_phoff); */
/*   CHECK_MMAP(mmptr, mmlen, phdr); */
/*   nphdr = ehdr->e_phnum; */

/*   for (n = 0; n < nphdr; ++n) */
/*     { */
/*       printf("[%03d]: [@0x%08lx - @0x%08lx]\n", n, (unsigned long)phdr[n].p_vaddr, (unsigned long)(phdr[n].p_vaddr + phdr[n].p_memsz)); */
/*     } */

/*   return 0; */
/* } */


/* Elf header related
 */

static void __attribute__((unused)) symbol_dump_fn(void* sym, void* unused)
{
  unused = 0;
  printf("%s\n", ft_symbol_dump((ft_symbol_t*)sym));
}



/* Match the elf header magic number
 */
ft_bool_t elf_match(char* mmptr, unsigned int mmlen)
{
  Elf32_Ehdr* ehdr;

  ehdr = (Elf32_Ehdr*)mmptr;
  CHECK_MMAP(mmptr, mmlen, ehdr);

  if ((*(unsigned long*)ehdr->e_ident) != *((unsigned long*)ELFMAG))
    return false;

  return true;
}


/* Explore the elf format
 */
ft_error_t elf_explore(struct ft_proc* proc, char* mp, unsigned int sz)
{
  return FT_ERR_SUCCESS;
}


/* Get all symbols
 */
ft_error_t elf_getsyms(struct ft_list** symfiles, ft_symfile_t* symfile)
{
  ft_list_t* sections;

  sections = ft_list_init();

  if (elf_sections_to_list(symfile->mmptr, symfile->mmlen, &sections) == 0 &&
      elf_sections_get_informations(symfile->mmptr, symfile->mmlen, sections) == 0 &&
      elf_get_exported_symbols(symfile, sections, &symfile->symlist) == 0 &&
      elf_get_dependencies(symfile->mmptr, symfile->mmlen, sections, &symfile->symlist, symfiles) == 0)
    ;

  /* ft_list_foreach(symfile->symlist, symbol_dump_fn, 0); */

  ft_list_release(&sections, section_release);
  return FT_ERR_SUCCESS;
}
