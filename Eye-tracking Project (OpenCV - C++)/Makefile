
PATH_TO_DIR = /home/IHM

CC = g++
EXEC = eye-tracking
INCLUDE1 = -I$(PATH_TO_DIR)/opencvfiles/include/opencv
INCLUDE2 = -I$(PATH_TO_DIR)/opencvfiles/include
LINK = -L$(PATH_TO_DIR)/opencvfiles/lib
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_video -lopencv_contrib -lopencv_objdetect

all : eye-tracking.cpp
	$(CC) $(INCLUDE1) $(INCLUDE2) $(LINK) -Wextra -g -o eye-tracking  eye-tracking.cpp -lX11 $(LIBS)

clean :
	rm -f *.o *~ *.a $(EXEC) *.hex *.elf
