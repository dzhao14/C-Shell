***This is my shell. My shell has LIMITED functionality which I'll describe below:***

To run the shell you can build it with `gcc -o nush nush.c` and then run it with `./nush`.

When running my shell, my shell can read at most two commands per line. (In other words, my shell can only execute ONE operator per line.)
My shell supports seven operators:
	<
	>
	|
	;
	||
	&&
	&

Because I am using strtok() to parse your input, operators must begin and end with a space.
	(e.g 'echo hi ; echo bye' is a valid command but 'echo hi; echo bye' won't work the way semicolon works)

I did not write the tests.

Other than that.... Have fun! 
