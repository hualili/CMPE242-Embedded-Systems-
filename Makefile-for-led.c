# ----------------------------------------------------------------------------
# Makefile for building tapp
#
# Copyright 2010 FriendlyARM (http://www.arm9.net/)
#

ifndef DESTDIR
DESTDIR			   ?= /tmp/FriendlyARM/mini6410/rootfs
endif

CFLAGS				= -Wall -O2
CC				= arm-linux-gcc
INSTALL				= install

TARGET				= led


all: $(TARGET)

led: led.c
	$(CC) $(CFLAGS) $< -o $@


install: $(TARGET)
	$(INSTALL) $^ $(DESTDIR)/usr/bin

clean distclean:
	rm -rf *.o $(TARGET)


# ----------------------------------------------------------------------------

.PHONY: $(PHONY) install clean distclean

# End of file
# vim: syntax=make

