

Prerequisites
-
1. Install `makedepend` by command `$ sudo apt-get install xutils-dev`
	1. The `Makefile` is partly generated by the `makedepend`. Whenever you add a new module to the project, remember to append the module references to the `Makefile` under `SRC` and `SRC_TEST`. Then run `make depend` to append new targets at the end of the `Makefile`
2. Install `CMocka` test framework. Instructions can be found from [here](https://cmocka.org/)
	1. Create a new folder and move into it
	2. `wget https://cmocka.org/files/1.1/cmocka-1.1.1.tar.xz`
	3. Untar the package `tar -xfv cmocka-1.1.1.tar`
	4. Go into the folder and execute the commands `cmake ./cmocka-1.1.1`, `make` and `sudo make install` (`sudo` is needed to install `cmocka` generally available)

Coding Style
-
The code is (loosly) following this [coding standard](https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html)


