## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS	= gnu
MAINTAINERCLEANFILES    = Makefile.in

INCLUDES = -I$(srcdir)

bin_PROGRAMS = gsm_emulator

gsm_emulator_SOURCES = gsm_emulator.c

gsm_emulator_CFLAGS = -I../../src -DDEBUG

gsm_emulator_LDADD = ../../src/.libs/libgsmc.a

