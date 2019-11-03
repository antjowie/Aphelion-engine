# Shinobu-engine
A game engine written in C++, made as an educational passion project.

I am using this project as a playground so nothing is really set in stone. This project heavily references [The Cherno his game engine series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT).

## How to build
Simply do the following in a command prompt/terminal (on Windows):
```
git clone https://github.com/antjowie/Shinobu-engine.git shinobu-engine
cd shinobu-engine

cmake -S . -B build
cmake --build build
```
The project makes use of submodules, but they are automatically pulled if `GIT_SUBMODULE` is on.