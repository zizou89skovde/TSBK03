COMMONDIR=common/
SOURCEDIR=src/

UNAME := $(shell uname)

#ifeq ($(UNAME), Linux)
CC=gcc
CPPFLAGS= -g -std=c++0x -lGL -DGL_GLEXT_PROTOTYPES -Wno-write-strings -I$(COMMONDIR) -I$(SOURCEDIR)
CFLAGS= -g -std=c99 -lGL -DGL_GLEXT_PROTOTYPES -Wno-write-strings -I$(COMMONDIR) -I$(SOURCEDIR)
LINK= -g -shared-libgcc -lstdc++ -lm -lGL -lpthread -DGL_GLEXT_PROTOTYPES -lX11 -I$(COMMONDIR) -I$(SOURCEDIR)
#endif

VPATH=common src
ANT_TWEAK=-LAntTweakBar/lib -lAntTweakBar
OBJFILES=CPUClothSimulation.o GPUSimulation.o SimulationClass.o GPUWaterSimulation.o GPUClothSimulation.o GrassSimulation.o main.o TerrainLOD.o Environment.o ModelObject.o KeyMouseHandler.o GL_utilities.o VectorUtils3.o loadobj.o LoadTGA.o MicroGlut.o PostProcessing.o CelShading.o milli.o

TARGET=main

.PHONY: all clean

$(TARGET): $(OBJFILES)  	
	$(CC) -o $(TARGET) $(OBJFILES) $(LINK)
%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $< 

%.o: %.c
	$(CC) -c $(CFLAGS)  $<

clean:
	@ \rm -f *.o $(SOURCEDIR)*.o *.gch $(COMMONDIR)*.o *~ $(COMMONDIR)*~ $(SOURCEDIR)*~ main
