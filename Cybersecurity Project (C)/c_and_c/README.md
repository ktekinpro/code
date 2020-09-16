# Command and Control Server : C&C

Communicate by Message Passing IPC with admin.c :
- Receive type 1 messages
- Answer by type 2 messages
- End the communication by sending a type 3 message

Communicate by Shared Memory IPC with the HTTP Server :  
- 1rst shm : read data written by the HTTP server
- 2nd shm : write data to make the HTTP server print it on the website page

Communicate by TCP with the botnets.

**PASSUPLOAD macro**  
By compiling with make passupload, the PASSUPLOAD macro is defined. When it is defined, the dynamic library will be uploaded only in the C&C, it will not be uploaded in any botnets via TCP.

**Issues :**  
Sometimes dialogue between C&C and botnets never ends.
