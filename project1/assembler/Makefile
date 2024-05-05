CC = gcc
OBJS = assembler.o
TARGET = assembler
 
.SUFFIXES : .c .o
 
all : $(TARGET)
 
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)
	rm -f $(OBJS)
 
clean :
	rm -f $(OBJS) $(TARGET)