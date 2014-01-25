1. Build userland

    cd src
    make

2. Build kernel module

    cd src
    make -f ./Makefile.kern

3. Static linked with your sources

    Headers reside in include/ dir.
    Sources reside in src/ dir.
