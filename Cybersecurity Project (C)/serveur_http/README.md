# HTTP Server

To communicate with the HTTP server :  
Launch ./serveur then go to : 127.0.0.1:4100/page_admin.html

Listen on TCP port 4100 and analyze the POST forms returned by page_admin.html

We can analysis the following forms :
- Upload a dynamic library
- Run a dynamic library
- Remove a charge
- Get the result of a charge's execution
- Ask for bot's statistics
- List botnets' IP
- List C&C's dynamic libraries

The data received are displayed on the terminal.

In the html pages (www directory), only the page_admin.html page could be coded.

Currently, we are able to receive a charge from the web browser, and to copy it entirely in the folder ../c_and_c/Charges_cc .  
We also transmit the bot ip on which we will send this charge.

In the lib_dynamique directory, there is the source code of our test charge as well as a Makefile to compile it.
