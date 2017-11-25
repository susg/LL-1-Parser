# LL(1) Parser

### Running the program
In order to run the program, write a simple c program in `input.c`. Then execute `lexical_analyser.c` as follows :
```sh
$ g++ lexical_analyser.c
$ ./a.out
```
This creates the `output_prog` and the `symbol_table` with scope and also `tokens`.
Now run `parser.cpp` as follows:
```sh
$ g++ parser.cpp
./a.out
```
This will store the output in the file `output`. It contains the stack position and whether the program in `input.c` is correct or not.
Grammar is in `grammar.txt`. Symbol table with scope is stored in `symbol_table`.

