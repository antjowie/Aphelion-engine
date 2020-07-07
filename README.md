# Shinobu-engine
A cross platform game engine written, made as an educational passion project.  

This project heavily references [The Cherno his game engine series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT).

## Purpose
Ultimately, I want to be able to use this engine for all my game development needs. I want to create a robus project in which I can easily prototype. I do not intend to compete with existing software. This is also quite a large project that I plan to work on for quite a while.

## Build instructions
The project makes use of CMake so building should be straightforward. Simply do the following in a command prompt/terminal (on Windows):
> NOTE: As of right now, the core module can't be build as a dll since it defines the entry point and the executable needs to have an entry point.

```
git clone https://github.com/antjowie/Shinobu-engine.git shinobu-engine
cd shinobu-engine

cmake -S . -B build
cmake --build build
```
The project makes use of submodules, but they are automatically pulled if `GIT_SUBMODULE` is on.

**[Important]** When working on the project be sure to build all targets. CMake is set up to copy all needed resources when building all targets. I have yet to figure out a better way of doing this.