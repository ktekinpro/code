# Local administration of the C&C

- Communication via Inter Process Communication (IPC) using Message Passing method.

- admin send a type 1 request and then wait for a type 3 answer to stop itself.

- on the other side the C&C send a series of type 2 messages.
To end the communication it finally send a type 3 message.

## Options :  

-c get C&C's Id
-l List botnets' IP  
-d <charge> <bot> start a charge on a bot (charge = dynamic library)  
-e <charge> <bot> remove a charge  
-r <index> <bot> get the result of a charge's execution  
-i list the dynamic libraries that was uploaded on the C&C  
-s <ip> print botnet's statistics  
-q <ip> quit the TCP communication with the bot  
-h return admin.c's documentation   

***For example, launch	: $ ./admin***  
***Or : $ ./admin -e charge.so 127.0.0.1***
