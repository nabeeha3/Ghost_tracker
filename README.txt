Name: 
Nabeeha Moazzam, 101272537
Justin Sauve, 101273273

Program’s purpose:
A map of connected rooms. Four hunters, each with a single device to take readings of evidence from a room. 
A ghost, moving around & leaving evidence for hunters to find. When a hunter is in a room with a ghost, their fear level goes from 0 to 100,
and when they reach 100, they leave! Every ghost leaves behind three distinct kinds of evidence. 
We get readings in every room; there’s also sounds and temperatures to track, but a ghost leaves behind a special form of evidence. 
If we can find all three of those special kinds of evidence, we can identify the ghost and get rid of it!


Files included and their purposes:
- README.txt  : (this file) provide some guidance on how to use the program, its purpose and description
- main.c : prints selections and takes user prompt, runs through entire program
- defs.h: forward declarations, type definitions, error codes and other definitions 
- ghost.c: code relating to ghost; handles most ghost functions
- logger.c: logging file, ensure major actions are recorded consistently
- house.c: creates the 'house', connects and adds rooms
- utils.c: converts some enum types to strings, has random generators
- hunter.c: code relating to hunters; handles most hunter functions


- Makefile: compilation



Instructions for compiling the program: make main
Instructions for running the program: ./main

Instructions for how to use the program once it is running: N/A view the images printed in terminal
