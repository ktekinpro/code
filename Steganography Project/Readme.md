# Steganography Project

***Keywords : cryptography, C, image analysis***

### Encryption

The "encrypte.c" program hides the string passed in parameter
into an image, and saves it in an output image.

First of all, the string is encrypted with the Vigenère cipher algorithm using the key which was given by the user.

Then we calculate a 'seed' for the srand function using a hash algortihm,
which for a given ASCII key calculates a corresponding integer.

Then, we call rand () to generate random positions allowing us to store
the message randomely.

To hide the message, we divide each character into 4 packets of 2 bits. These 2 bits will be stored randomly (according to the seed which was given in the srand function) on the least significant bits of the blue pixels.

Finally, the image with the hidden message is saved in "img-out.ppm"


### Decryption

The "encrypte.c" program finds the encrypted message hidden in the image passed in parameter.

First, we calculate the hash key of our Vigenère key, and we call srand by passing this value as a parameter.

Then we continue generating a sequence of random positions until we have discovered
the ASCII end-of-string character '\ 0'.

Finally, we decrypt our message using the Vignère cipher.

At the end, the hidden message is printed on the standard output.


### How it works

Here are the commands to use the program :

~$ make  
~$ ./encrypte \<input_image.ppm\> \<Vigenère_key\> \<ascii_message\> \<output_image.ppm\>  
~$ ./decrypte \<input_image.ppm\> \<Vigenère_key\>  

* the program works only with ppm P3 ASCII images
* the message to hide must be ASCII, composed only by lowercase letter, and its size must be less than 500 characters
* the Vigenère key must be ASCII, composed only by lowercase letter, and its size must be less than 500 characters

Example :  
~$ ./encrypte img.ppm mykey mymessagetohide img-out.ppm    
~$ ./decrypte img-out.ppm mykey  


### Documentation

For more informations :  
~$ ./encrypte -h  
~$ ./decrypte -h  


### Credits

http://fvirtman.free.fr/recueil/01_09_01_testppm.c.php  
https://www.thecrazyprogrammer.com/
