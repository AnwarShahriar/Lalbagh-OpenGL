## Lalbagh Fort in 3D (OpenGL)

This is an old OpenGL project showing a 3D rendered environment of Lalbagh Fort I had done as an undergrad student. Long ago I lost the source codes of this piece of Art ;) (I didn't know about git back then). I've found it again recently as an attachment in one of my old email account and thought why not share it with the world :) (That's why you'll see that all code is comitted under the first commit) Please feel free to move around and fork it as you like.

#### Here's a Wikipedia link about [Lalbagh Fort](https://en.wikipedia.org/wiki/Lalbagh_Fort)

> Previously this project `windows.h` header file (I was using Windows back then) but now the dependency is removed and the code is made compatible only for Linux (because now all my machines run on Linux). I've tested it with Ubuntu 16.04 LTS version.

To Build this code you need the following dependencies: (This is for Ubuntu or debian based system)
1. freeglut3 (sudo apt-get install freeglut3)
2. freeglut3-dev (sudo apt-get install freeglut3-dev)
3. libglui2c2 (Install it from [here](http://www.rpmseek.com/rpm-pl/libglui2c2.html?hl=com&cs=libgcc1:RE:0:0:0:0:2420))
4. libglui-dev (Install it from [here](http://www.rpmseek.com/rpm-pl/libglui-dev.html?hl=com&cx=589:L:1000))

Now run the following commands,
```shell
> g++ lalbagh.cpp -lGL -lGLU -lglut -g -o lalbagh
> ./lalbagh
```
**And Voila!**

![alt Rendered image of Lalbagh Fort](https://github.com/AnwarShahriar/Lalbagh-OpenGL/blob/master/screenshots/lalbagh.png "Project Lalbagh")

**Features not working right now:**
1. Camera movement (Will be fixed when I get some free time)
