# Dictionary implementation and text correction in C language

***Keywords :  C, tree data structure, time optimization***  

**Advanced Programming & Data structure  
6th semester of university studies  
Department of Computer Science and Automation,  
Electronic and Electrical Engineering**


## Overview

*The comments and the interface are in French  
Text file and dictionnary are in English*

Welcome and thank you for using our project.

In this readme you will find the basic commands
for the execution of our program.

For that, you will need a dictionary with every word separated by '\n'
And a file to correct

Note that only words in ASCII are analyzed, and more specifically,
uppercase / lowercase letters from 'a' to 'z' and apostrophes, typically text written in English.

You will find an example of a file to correct and a dictionary with the source code :

dico.txt : English word dictionary, converts to ASCII
mon_texte.txt : some paragraphs of Wikipedia in English


## Compilation

First, you have to compile the source code in order to run it

To do this, go to the directory of the project where the MakeFile is located and execute the following command:

~$ make


## Execution

To launch our command line program, please respect the following structure:

./[binary] [dictionary] [text_to_correct]

For example :

~$ ./main dico.txt mon_texte.txt



## Asking for help

To get some help, please type :

~$ ./main -h


## Process #

The program displays on the standard output the words of the text file
which are not present in the dictionary.

You will also have the position of the unknown word, as well as the total number
of words that are not present in the dictionary.
