SRCS=framebuffer_settings.c
OBJS=$(SRCS:.o=.c)

CC=gcc
CFLAGS=-O2 -Wall
LDFLAGS=-lm

all: framebuffer_settings gensoa

%.o:%.c
	$(CC) $(CFLAGS) $< -c -o$@

framebuffer_settings: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

gensoa: gensoa.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
