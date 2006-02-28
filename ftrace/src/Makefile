##
## Makefile for  in /home/texane/texane/wip/ftrace/src
## 
## Made by 
## Login   <texane@epita.fr>
## 
## Started on  Sat Jan 14 11:53:19 2006 
## Last update Sat Jan 21 00:43:55 2006 
##


# This is a gnu Makefile

# target name, should use uname -p instead of uname -m... doesn't work on inca ;)
swdep	:=	$(shell uname -s)
hwdep	:=	$(shell uname -m)
target	:=	$(swdep)-$(hwdep)

# {in,out}put files
name	:=	ftrace_$(target)
srcs	:=	main.c		\
		ft_proc.c	\
		ft_event.c	\
		ft_watchpoint.c	\
		ft_bktrace.c	\
		ft_frame.c	\
		ft_symbol.c	\
		ft_list.c	\
		ft_mmloc.c	\
		ft_cmd.c	\
		fmt/elf.c

cflags	:=	-Wall -Iinclude/

# software dependecies
ifeq ($(swdep), Linux)
cflags	+=	-DFT_PROC_USE_PTRACE
srcs	+=	swdep/ptrace.c
endif

ifeq ($(swdep), NetBSD)
cflags	+=	-Wstrict-prototypes
cflags	+=	-DFT_PROC_USE_PTRACE
srcs	+=	swdep/ptrace.c
endif

ifeq ($(swdep), SunOS)
cflags	+=	-DFT_PROC_USE_PROCFS
srcs	+=	swdep/procfs.c
endif

ifeq ($(swdep), HP-UX)
cflags	+=	-DFT_PROC_USE_TTRACE
srcs	+=	swdep/ttrace.c
endif


# hardware dependecies
ifeq ($(hwdep), i386)
cflags	+=	-DFT_ARCH_IA32
srcs	+=	hwdep/watchpoint_ia32.c		\
		hwdep/frame_ia32.c
endif

ifeq ($(hwdep), ia64)
cflags	+=	-DFT_ARCH_IA64
srcs	+=	hwdep/watchpoint_ia64.c		\
		hwdep/frame_ia64.c
endif

ifeq ($(hwdep), sun4u)
cflags	+=	-DFT_ARCH_SPARCV9
srcs	+=	hwdep/watchpoint_sparcV9.c	\
		hwdep/frame_sparcv9.c
endif


objs	:=	$(srcs:.c=.o)
garbage	:=	$(srcs:.c=.c~) \
		include/*~ \
		*.core \
		Makefile~


# rules
.PHONY	:	all clean fclean re install safety depend .mkdeps
.SUFFIX	:	.c .o .h .a
all	:	$(name)
$(name)	:	$(objs)
		gcc -o $(name) $(objs)
%.o	:	%.c
		gcc $(cflags) -c $< -o $@
clean	:
		-@rm -rf $(objs)
		-@rm -rf $(garbage)
fclean	:	clean
	  	-@rm -rf $(name)
re	:	fclean all
