# CS50 CHESSBOT

### [VIDEO DEMONSTRATION](https://www.youtube.com/watch?v=fs7WaSePN8Q&t=5s)

### My information:

#### My name is Thomas Smallwood, my github is www.github.com/davedude1011,
#### born in cyprus, living in robertsbridge england,
#### the current date is sunday the 13th of april 2025

### Project information:

#### This Project is a Chess playing agent that uses the common Minimax algorithm for move tree searching, and its optimized using bitboard anotation.

### Bitboards
#### Bitboards are 64 bit unsigned integers used (in this context) to store positional data on a chessboard, with each bit represending a square, and the value of that bit representing concept of that square being either occupied or not. Due to the limitations of binary only having 2 possible values per bit, a different bitboard is needed to store the positions of each type of piece on both teams, so in total to represent the entire chessboard, 12 bitboards (64 bit unsigned integers) are needed, so simplify this i made a custom chessboard struct object that holds each of them and can be easily passed to the print_board function for pretty printing.
#### The benefits of this added complexity in piece storage is speed, theoretically it should take a single CPU cycle to get the position of a piece, 2 or 3 to move a piece etc, the entire chessboard is basically just 12 very large numbers stored in RAM that are interpereted as board positions, i found this to be a pretty interesting concept to think about both from an abstract point of view looking at it from the aspect of designing the program, as well as logically thinking about the memory utilization in wich their just 12 numbers, an interesting conciquence to this was *many* bugs in which one or more of the bitboards i would *forget or whatnot* to initialize and they would be garbage values, which visually looked like a lovecraftian combination of different pieces where they definatly shouldnt have been.

### Minimax
#### The main algorithm that powers this bot, and most other chess engines even at the highest levels, is Minimax, given a tree like structure it can recursively navigate through that tree, and given a method of serializing a position (node of the tree its navigating) it can get the "value" of each position, by minimizing the "value" of the position for black and maximizing the position for white, (thats where the name Minimax comes from), you can find the best possible path in the tree, although this requires computing every single combination of moves, which is why a depth is given, the default depth on my program is 3, which means the search tree given to minimax is every movable position from every movable position from every movable position, a list of moves which can get stupidly large in a game like chess, but with my bitboard optimizations a depth of 3 is realistically instant, a depth of 5 is when it starts taking 6 - 12 seconds per move (this is all subjective of coarse, because it depends on the CPU running it), i did have the thought at some point *to just offload the work to the gpu* but that thought left quicker than it came :I

### UI
#### i used some website to generate ASCII art for the "CS50 CHESS!" intro message, and i added options for player vs engine, engine vs engine, player vs player, and changing the depth the engine searches at, and a quit button
