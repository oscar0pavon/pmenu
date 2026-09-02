CC = cc

# pmenu draws through wl_shm and pfonts' cpu backend and calls no gl or egl
# function itself. -lGL and -lEGL are still needed at link time: libpway.a
# keeps its egl backend and libpfonts.a its gl one as unused alternatives to
# the cpu paths this binary actually calls, and a static archive resolves the
# symbols of every object it pulls in
LIBS = -lm -lpway -lpfonts
LIBS += -lGL -lEGL -lwayland-client -lwayland-egl
LIBS += -lxkbcommon
# libpfonts' cpu backend composites glyphs with pixman
LIBS += $(shell pkg-config --libs pixman-1)

FLAGS = -g -I/usr/local/include $(shell pkg-config --cflags pixman-1)
LDFLAGS = -L/usr/local/lib $(LIBS)

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: pmenu

.c.o:
	$(CC) $(FLAGS) -c $<

$(OBJ): config.h pmenu.h

pmenu: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

install: pmenu
	cp -f pmenu /usr/bin
	cp -f pmenu_run /usr/bin

clean:
	rm -f pmenu $(OBJ)

.PHONY: all install clean
