#include <stdio.h>
#include <string.h>
#include <unistd.h>

void start(){
    write(1,"Hello stdout I'm here !\n",24);//write in stdout
    write(2,"Hello stderr here is my error !\n",32);//write in stderr
}
