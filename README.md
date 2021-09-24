# Small World Networks

This is the repository where we will keep our C++ code for our small world
networks and COVID spread research.

## Project structure

The project is set up to build using CMake. Our code is organized into
"modules", where each module has its own folder and provides either a single
library or an executable. This structure allows sharing of some common code,
such as our IO code, between subprojects, while still keeping the projects
themselves separate.

## Building

Make sure you have CMake version 3.18 or greater installed. In addition, you'll
need a relatively modern C++ compiler such as g++.

Run the following commands to build the project:

```
mkdir build
cd build
cmake ..
make
```

Optionally, you may specify a particular target in the `make` command (for
example, `make simulation`) to build only that target. A folder called "bin"
will be created in the project root, and all compiled binaries will be placed 
in that folder.
