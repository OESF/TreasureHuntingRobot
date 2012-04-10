CC = g++
CFLAGS = -MD -MP -MT -c -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp  -O3 -fno-tree-pre -fno-strict-aliasing -ftree-vectorize -ffast-math -funsafe-math-optimizations -fsingle-precision-constant -O2
LDFLAGS =
INCLUDES = -I/usr/include/ni
LIBS = -lOpenNI
TARGET = NiSkeletonServer
OBJS = TcpServer.cpp NiSkeletonServer.cpp 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	-rm -f $(TARGET) *.d *.o

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<
