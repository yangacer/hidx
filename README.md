# CMake build instructions

```
cmake -S . -B out
cmake --build out
# [Optional]
cmake --build out --target test
```

# Legacy build instructions

1. Build for userland

    cd src
    make

2. Build for FreeBSD kernel module

    cd src
    make -f ./Makefile.kern

3. Static linked with your sources

    Headers reside in include/ dir.
    Sources reside in src/ dir.
