# Assignment 4

Purpose: To develop a real, working web server. The web server would be multi-threaded so that it will be more efficient.

server.c

Contains main() for the web server. The multi-threaded web server is implemented using a master/worker thread that are in a producer-consumer relationship.

request.c

Performs most of the work for handling requests in the basic web server. All procedures in this file begin with the string “request”.

cs537.c

Contains wrapper functions for the system calls invoked by the basic web server and client.

client.c

Contains main() and the support routines for the very simple web client.

output.c

Code for a CGI program. Basically, it spins for a fixed amount of time, which you may useful in testing various aspects of your server.