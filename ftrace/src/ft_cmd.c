/*
** ft_cmd.c for  in /home/texane/texane/school/ept3/ftrace/src
** 
** Made by 
** Login   <texane@epita.fr>
** 
** Started on  Sat Jan 21 00:43:40 2006 
** Last update Sat Jan 21 12:55:56 2006 fabien le-mentec
*/


#include <ft_cmd.h>
#include <ft_proc.h>
#include <ft_list.h>
#include <ft_error.h>
#include <ft_basic_types.h>


/* Internal routines
 */

static inline void outstr(const char* str)
{
  write(1, str, strlen(str));
}

char** parseline(char* line)
{
  char** av;
  char* ptr;
  char* prev;
  unsigned int n;
  unsigned int k;

  /* Get token count */
  n = 0;
  ptr = line;
  while (*ptr)
    {
      if (*ptr == ' ' || !*(ptr + 1))
	{
	  *ptr = 0;
	  ++n;
	}
      ++ptr;
    }

  /* Fill av */
  av = malloc((n + 1) * sizeof(char*));
  av[n] = 0;
  for (k = 0; k < n; ++k)
    {
      prev = line;
      while (*line)
	++line;
      ++line;
      av[k] = prev;
    }

  return av;
}


/* Understood commands
 */

typedef struct
{
  char* label;
} wpt_args_t;

typedef struct
{
  char* path;
} symfile_args_t;

typedef struct
{
  char** av;
} tracee_args_t,
  ls_args_t,
  setsym_args_t;

typedef struct ft_cmd
{
  void* args;
  ft_error_t (*release_fn)(struct ft_cmd*);
  ft_error_t (*exec_fn)(struct ft_cmd*, ft_proc_t**);
} ft_cmd_t;

static ft_cmd_t* create_cmd(void)
{
  ft_cmd_t* cmd;

  cmd = malloc(sizeof(ft_cmd_t));
  if (cmd)
    {
      cmd->args = 0;
      cmd->release_fn = 0;
      cmd->exec_fn = 0;
    }
  return cmd;
}


/* Execution functions */

static ft_bool_t tok_isnum(const char* tok)
{
  while (*tok)
    {
      if ((*tok < '0') || (*tok > '9'))
	return false;
    }
  return true;
}

extern char** environ;
static ft_error_t exec_set_tracee(ft_cmd_t* cmd, ft_proc_t** proc)
{
  ft_error_t err;
  tracee_args_t* args;
  int ac;

  err = FT_ERR_SUCCESS;

  args = (tracee_args_t*)cmd->args;
  for (ac = 0; args->av[ac]; ++ac)
    ;

  if (ac == 0)
    {
      err = FT_ERR_BADARGS;
      goto eoexec;
    }

  /* Guess the tracee is a running process */
  if (tok_isnum(args->av[0]) == true)
    {
      if (ac != 1)
	{
	  err = FT_ERR_BADARGS;
	  goto eoexec;
	}
      err = ft_proc_init_from_pid(proc, atoi(args->av[0]));
    }
  else
    {
      err = ft_proc_init_from_av(proc, args->av, environ);
    }

 eoexec:
  return err;
}

static ft_error_t exec_set_symfile(ft_cmd_t* cmd, ft_proc_t** proc)
{
  symfile_args_t* args;

  if (!*proc)
    return FT_ERR_BADPROC;

  args = (symfile_args_t*)cmd->args;
  ft_symbol_file_add(*proc, args->path);
  return FT_ERR_SUCCESS;
}

static ft_error_t exec_set_watchpoint(ft_cmd_t* cmd, ft_proc_t** proc)
{
  wpt_args_t* args;

  if (!*proc)
    return FT_ERR_BADPROC;

  args = (wpt_args_t*)cmd->args;
  ft_watchpoint_add(*proc, args->label, 0, false, FT_WATCH_ALL);
  return FT_ERR_SUCCESS;
}

static ft_error_t exec_do_continue(ft_cmd_t* cmd, ft_proc_t** proc)
{
  if (!*proc)
    return FT_ERR_BADPROC;

  (*proc)->do_prompt = false;
  return FT_ERR_SUCCESS;
}


static ft_error_t exec_ls_sym(ft_cmd_t* cmd, ft_proc_t** proc)
{
  if (!*proc)
    return FT_ERR_BADPROC;
  return FT_ERR_SUCCESS;
}


static void print_watchpoint(ft_watchpoint_t* wpt, void* unused)
{
  printf("%s\n", ft_watchpoint_dump(wpt));
}

static ft_error_t exec_ls_watchpoint(ft_cmd_t* cmd, ft_proc_t** proc)
{
  if (!*proc)
    return FT_ERR_BADPROC;
  ft_list_foreach((*proc)->watchpoints, (void(*)(void*, void*))print_watchpoint, 0);
  return FT_ERR_SUCCESS;
}


/* Releasing functions */




/* Extraction function */

static ft_error_t get_set_symfile(ft_list_t** cmdlist, char** av, int* skip)
{
  /* $> set-symfile <filename> */
  ft_cmd_t* cmd;
  symfile_args_t* args;

  *skip = 1;
  cmd = create_cmd();
  if (cmd)
    {
      args = malloc(sizeof(symfile_args_t));
      if (args == 0)
	return FT_ERR_MALLOC;
      args->path = strdup(*av);
      cmd->args = (void*)args;
      cmd->exec_fn = exec_set_symfile;
      ft_list_push_back(cmdlist, (void*)cmd);
    }

  return FT_ERR_SUCCESS;
}

static ft_error_t get_set_tracee(ft_list_t** cmdlist, char** av, int* skip)
{
  /* $> set-tracee <filename> | <pid> */

  ft_cmd_t* cmd;
  tracee_args_t* args;
  int ac;

  for (ac = 0; av[ac]; ++ac)
    ;
  if (ac == 0)
    return FT_ERR_BADARGS;

  *skip = ac;

  /* The first command is used to set the tracee */
  cmd = create_cmd();
  if (cmd)
    {
      args = malloc(sizeof(tracee_args_t));
      if (args == 0)
	return FT_ERR_MALLOC;
      args->av = malloc((ac + 1) * sizeof(char*));
      if (args->av == 0)
	return FT_ERR_MALLOC;
      args->av[ac] = 0;
      for (ac = 0; av[ac]; ++ac)
	args->av[ac] = strdup(av[ac]);
      cmd->args = (void*)args;
      cmd->exec_fn = exec_set_tracee;
      ft_list_push_front(cmdlist, (void*)cmd);
    }

  /* The second command is used to set a symbol file, if any */
  if (tok_isnum(av[0]) == false)
    get_set_symfile(cmdlist, av, &ac);

  return FT_ERR_SUCCESS;
}

static ft_error_t get_set_watchpoint(ft_list_t** cmdlist, char** av, int* skip)
{
  /* $> set-watchpoint {x|r|w} <address> | <symbol> */
  ft_cmd_t* cmd;
  wpt_args_t* args;

  *skip = 1;
  cmd = create_cmd();
  if (cmd)
    {
      args = malloc(sizeof(wpt_args_t));
      if (args == 0)
	return FT_ERR_MALLOC;
      args->label = strdup(*av);
      cmd->args = (void*)args;
      cmd->exec_fn = exec_set_watchpoint;
      ft_list_push_back(cmdlist, (void*)cmd);
    }

  return FT_ERR_SUCCESS;
}

static ft_error_t get_do_continue(ft_list_t** cmdlist, char** av, int* skip)
{
  /* $> do-continue (address) */
  ft_cmd_t* cmd;

  *skip = 0;
  cmd = create_cmd();
  if (cmd)
    {
      cmd->exec_fn = exec_do_continue;
      ft_list_push_back(cmdlist, (void*)cmd);
    }

  return FT_ERR_SUCCESS;
}

static ft_error_t get_ls_watchpoint(ft_list_t** cmdlist, char** av, int* skip)
{
  *skip = 0;

  ft_cmd_t* cmd;

  *skip = 0;
  cmd = create_cmd();
  if (cmd)
    {
      cmd->exec_fn = exec_ls_watchpoint;
      ft_list_push_back(cmdlist, (void*)cmd);
    }

  return FT_ERR_SUCCESS;
}

static ft_error_t get_set_sym(ft_list_t** cmdlist, char** av, int* skip)
{
  return FT_ERR_NOTIMPL;
}

static ft_error_t get_ls_sym(ft_list_t** cmdlist, char** av, int* skip)
{
  *skip = 0;

  ft_cmd_t* cmd;

  *skip = 0;
  cmd = create_cmd();
  if (cmd)
    {
      cmd->exec_fn = exec_ls_sym;
      ft_list_push_back(cmdlist, (void*)cmd);
    }

  return FT_ERR_SUCCESS;
}



struct
{
  const char* long_name;
  const char* short_name;
  const char* comments;
  ft_error_t (*get_fn)(ft_list_t**, char** av, int* skip);
} g_cmd[] =
  {
    /* Tracing related */
    {"set-tracee", "-t", "", get_set_tracee},
    {"do-continue", "c", "", get_do_continue},

    /* Watchpoints related */
    {"set-watchpoint", "-w", "", get_set_watchpoint},
    {"ls-watchpoint", "lw", "", get_ls_watchpoint},
    
    /* Symbol related */
    {"set-symfile", "-sf", "", get_set_symfile},
    {"ls-sym", "ls", "", get_ls_sym},
    {"set-symbol", "-su", "", get_set_sym},

    {0, 0, 0, 0}
  };



/* Exported
 */

ft_error_t ft_cmd_interact_with_user(ft_proc_t** proc)
{
#define MAXBUF 512
  char line[MAXBUF];
  char** av;
  ft_list_t* cmdlist;

  /* Not yet initialized */
  while (*proc == 0)
    {
      outstr("$(tracee-not-set)>");
      if (fgets(line, sizeof(line), stdin) == 0)
	return FT_ERR_BADPROC;
      av = parseline(line);
      if (av)
	{
	  ft_cmd_list_from_av(&cmdlist, av);
	  ft_cmd_exec_list(cmdlist, proc);
	  free(av);
	}
    }

  /* Not in interactiv mode */
  if ((*proc)->interactiv == true)
    return FT_ERR_BADSTATE;

  /* Serve user */
  (*proc)->do_prompt = true;
  while ((*proc)->do_prompt == true)
    {
      outstr("$>");
      if (fgets(line, sizeof(line), stdin) == 0)
	(*proc)->do_prompt = false;
      av = parseline(line);
      if (av)
	{
	  ft_cmd_list_from_av(&cmdlist, av);
	  ft_cmd_exec_list(cmdlist, proc);
	  free(av);
	}
    }

  return FT_ERR_SUCCESS;
}


ft_error_t ft_cmd_list_from_av(ft_list_t** list, char** av)
{
  unsigned int n;
  unsigned int ncmd;
  unsigned int skip;
  unsigned char found;

  *list = ft_list_init();
  found = 0;
  for (n = 0; av[n]; ++n)
    {
      /* Try to match long names first */
      skip = 0;
      for (ncmd = 0; g_cmd[ncmd].long_name && (found == 0); ++ncmd)
	{
	  if (strcmp(av[n], g_cmd[ncmd].long_name) == 0 ||
	      strcmp(av[n], g_cmd[ncmd].short_name) == 0)
	    {
	      g_cmd[ncmd].get_fn(list, av + n + 1, &skip);
	      found = 1;
	      n += skip;
	    }
	}
    }

  return FT_ERR_SUCCESS;
}


static void execute_cmd_fn(ft_cmd_t* cmd, ft_proc_t** proc)
{
  if (cmd->exec_fn)
    cmd->exec_fn(cmd, proc);
}

static void __attribute__((unused)) release_cmd_fn(ft_cmd_t* cmd)
{
  if (cmd->release_fn)
    cmd->release_fn(cmd);
  free(cmd);
}


ft_error_t ft_cmd_exec_list(ft_list_t* list, ft_proc_t** proc)
{
  ft_list_foreach(list, (void(*)(void*, void*))execute_cmd_fn, (void*)proc);
  return FT_ERR_SUCCESS;
}
