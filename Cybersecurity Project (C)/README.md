# Cybersecurity Project : "Command and Conquer"

***Keywords : TCP, UDP, botnets, servers, Inter Process Communication (IPC), C langage, HTTP***

*More than 4 thousand lines of codes written in C.*


##      Aim of the project

The goal is to develop a set of libraries and programs allowing to communicate with different processes locally and remotely. Administrators will be able to manage multiple botnets and send them dynamic libraries, in order to make botnets do what administrators want (for more details please refer to "Features").


##      Project Architecture

This project is divided in several libraries and programs :

- Admin : local administrator, which communicates with the Command and Control server (C&C) by Inter Process Communication (IPC) using Message Passing method.

- Bot : botnet program, which communicates by UDP broadcast and TCP server.

- Serveur HTTP : TCP server, which communicates with the C&C by Inter Process Communication (IPC) using Shared Memory (shm) method.

- C&C : Command and Control server, which connects the local administrator and the HTTP server to the botnets.

- Lib_flux_exec : library for threads and mutexes.

- Lib_ipc : library for both Message Passing and Shared Memory IPC methods.

- Lib_reseau : library for UDP et TCP communication (for both Client and Server).


##        Compilation

Makefile's targets :
- all : compile all the libraries and programs
- clean : clean all the compiled files
- debug : compile with gcc -g flag
- passupload : compile the C&C by defining the PASSUPLOAD macro. When PASSUPLOAD is defined, the dynamic library will be uploaded only in the C&C, it will not be uploaded in any botnets via TCP.


##          Features

With the local administrator, we can :  

-c get C&C's Id  
-l List botnets' IP  
-d <charge> <bot> start a charge on a bot (charge = dynamic library)  
-e <charge> <bot> remove a charge  
-r <index> <bot> get the result of a charge's execution  
-i list the dynamic libraries that was uploaded on the C&C  
-s <ip> print botnet's statistics  
-q <ip> quit the TCP communication with the bot  
-h return admin.c's documentation   


With the remote administrator (HTTP server), we can : :  

-upload a charge to the C&C and/or on a specific botnet

To run the server, launch ./serveur and open a navigator, then go to : 127.0.0.1:4100/page_admin.html


##        Limits & Bugs

Our server analyzes several forms, but only communication with the C&C for uploading a dynamic library has been completed.

The TCP communication between the C&C and botnets sometimes freezes, we have analyzed the packets, they all end, a priori, with '\n'.

The local administrator just knows if the ip that user has entered corresponds to a bot detected by the C&C.   
It does not know, for example, if a dialogue has not ended or if the botnet could not perform one of its action.
If you want to know such things, you have to check the shell terminal that have launched the C&C.
