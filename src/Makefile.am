## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

AM_CFLAGS =\
	 -Wall\
	 -g

sbin_PROGRAMS = netustad
bin_PROGRAMS = netustadpasswd

common_src = \
	netustad.c\
	network.c\
	execcgi.c\
	acceptrequest.c\
	auth.c\
	loadconfig.c\
	mimetype.c\
	snprintf.c
	

if OS_LINUX
os_dep_src = \
	linuxaddrule.c \
	linuxdelrule.c \
	linuxeditrule.c
endif
if OS_FREEBSD
os_dep_src = \
	freebsdaddrule.c \
	freebsddelrule.c \
	freebsdeditrule.c
endif
netustad_SOURCES = $(common_src) $(os_dep_src)
netustadpasswd_SOURCES = netustadpasswd.c snprintf.c

netustad_LDFLAGS = 

netustad_LDADD = 

