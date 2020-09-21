ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: spacecandy.exe

spacecandy.exe: spacecandy.o
	gcc -o spacecandy.exe spacecandy.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

spacecandy.o: spacecandy.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c spacecandy.c

clean:
	del spacecandy.o 
	del spacecandy.exe