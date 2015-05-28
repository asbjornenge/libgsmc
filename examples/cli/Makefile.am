## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS	= gnu
MAINTAINERCLEANFILES    = Makefile.in

INCLUDES = -I$(srcdir)

bin_PROGRAMS = phone_cli

phone_cli_SOURCES = phone_cli.c

phone_cli_CFLAGS = -I../../src

phone_cli_LDADD = ../../src/.libs/libgsmc.a


