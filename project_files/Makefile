#Makefile by Carl Stejmar, 2014-09-11

COMMONDIR = common/
SOURCEDIR = src/
OBJDIR 	  = obj/
# Determine system and compiler
UNAME := $(shell uname)

DEFINE_FLAGS=$(if $(CPU_CLOTH),-DCPU_CLOTH,)
DEFINE_FLAGS+=$(if $(GPU_CLOTH),-DGPU_CLOTH,)

ifeq ($(UNAME), Darwin) # Mac
CXX     = gcc
CCFLAGS = -g -std=c++11 -stdlib=libc++ -pedantic -Wall -Wextra -I$(COMMONDIR)
CFLAGS  = -g -std=c99 -pedantic -Wall -Wextra -I$(COMMONDIR)
LINK    = -g -shared-libgcc -lstdc++ -framework OpenGL -framework Cocoa -I$(COMMONDIR)
GLUTPATH = $(COMMONDIR)mac
GLUT	= $(COMMONDIR)MicroGlut.m
endif
ifeq ($(UNAME), Linux)
CXX     = gcc
CCFLAGS = -g -std=c++0x -lGL -DGL_GLEXT_PROTOTYPES -I$(COMMONDIR) -I$(SOURCEDIR)
CFLAGS  = -g -std=c99 -lGL -DGL_GLEXT_PROTOTYPES -I$(COMMONDIR) -I$(SOURCEDIR)
LINK    = -g -shared-libgcc -lstdc++ -lm -lGL -lpthread -DGL_GLEXT_PROTOTYPES -lX11 -I$(COMMONDIR)
GLUTPATH = $(COMMONDIR)linux
GLUT	= $(COMMONDIR)MicroGlut.c
endif

CCFILES = $(SOURCEDIR)CPUClothSimulation.cpp $(SOURCEDIR)GPUClothSimulation.cpp $(SOURCEDIR)GrassSimulation.cpp $(SOURCEDIR)main.cpp $(SOURCEDIR)Terrain.cpp $(SOURCEDIR)ModelObject.cpp $(SOURCEDIR)KeyMouseHandler.cpp 
UTILS = $(COMMONDIR)GL_utilities.c $(COMMONDIR)VectorUtils3.c $(COMMONDIR)loadobj.c $(COMMONDIR)LoadTGA.c $(COMMONDIR)MicroGlut.c
OFILES = $(SOURCEDIR)CPUClothSimulation.o $(SOURCEDIR)GPUClothSimulation.o $(SOURCEDIR)GrassSimulation.o $(SOURCEDIR)main.o $(SOURCEDIR)Terrain.o $(SOURCEDIR)ModelObject.o KeyMouseHandler.o $(COMMONDIR)GL_utilities.o $(COMMONDIR)loadobj.o $(COMMONDIR)LoadTGA.o $(COMMONDIR)VectorUtils3.o $(COMMONDIR)MicroGlut.o


main: $(OFILES)
	$(CXX) $(DEFINE_FLAGS) $(OFILES) $(LINK) -o main

..common/%.o: $(UTILS)
	$(CXX) -c $(UTILS) $(CFLAGS) $< 

%.o: %.cpp
	$(CXX) -c $(CCFILES) $(CCFLAGS) $<

clean:
	@ \rm -f *.o *.gch ../common/*.o ../common/mac/*.o ../common/linux/*.o main

zap: clean
	@ \rm -f *~ ../common/*~


