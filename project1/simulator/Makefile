CC = gcc
OBJS = simulator.o
TARGET = simulator
 
.SUFFIXES : .c .o
 
all : $(TARGET)
 
$(TARGET): $(OBJS)
	   $(CC) -o $@ $(OBJS)
	   rm -f $(OBJS)
 
clean :
	rm -f $(OBJS) $(TARGET)
