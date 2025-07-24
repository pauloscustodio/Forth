Forth
-----

This is a C implementation of a ANS Forth interpreter. It implements all the 
CORE and CORE-EXT words. It is intended to be be used as a script interpreter, 
i.e. calling ABORT exits the interpreter instead of restarting the QUIT loop.

Why another Forth interpreter? Just for fun!

Output of WORDS:
```
. THROW DROP DUP PICK + .S WORDS S" TYPE ENVIRONMENT?
```

Copyright (c) Paulo Custodio, 2020-2025

License: [GPL3](https://www.gnu.org/licenses/gpl-3.0.html) 
