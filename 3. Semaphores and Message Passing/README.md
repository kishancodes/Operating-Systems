Author: Kishan Bhakta

How to create executable file:

$ make

How to run the program:

./runsim [-t sec] n < testing.data

How to uninstall the program:

$ make clean

Implemented runsim as follows:

Execute the following main loop until end-of-file is reached on stdin.

(a) Read a line from stdin of up to MAX_CANON characters (fgets); MAX_CANON is defined in the include file limits.h.

(b) Fork a child that calls docommand. docommand will request a license from the license manager object. Notice
that if the license is not available, the request function will go into a wait state.

(c) Pass the input string from step (a) to docommand. The docommand function will execl the specified command.

(d) The parent (runsim) checks to see if any of the children have finished (waitpid with WNOHANG option). It will returnlicense when that happens.

(e) After encountering EOF on stdin, wait for all the remaining children to finish and then exit. You will break the main loop when this happens.
