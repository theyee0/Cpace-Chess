# Cpace-Chess
A space chess engine written in ANSI C conforming to the rules of Raumschach

# Demo

https://github.com/user-attachments/assets/d9758eb4-3cc5-4314-aedf-ace23ecf7b94


# Usage

If you have an executable, you can run it on the command line:
```
./cpace_chess
```
Then, you'll be booted into a prompt inspired by the UCI chess protocol. You can set a position, reset the board, search for the best move, and quit the game at this point.

If you want a semi-interactive experience, I suggest you start with `ucinewgame` then begin searching with `go`. After ~30 seconds of searching, the program will output the best move it found. Then, using `position current <the move>`, you can perform the move. Finally, to view the board, you can use `printboard`.

# Building

If you don't want to build it yourself, you can download the program (built for x86-64 linux) from the Releases section.

A Makefile is provided for building. If your system supports some variant of GNU make, then you should be able to build it. Since the program is written in ANSI C and there aren't any external dependencies, It should (fingers crossed!) build properly regardless of your platform. No guarantees, though.
