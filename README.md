# slop
slop (Select Operation) is an application that queries for a selection from the user and prints the region to stdout.

## Features
* Hovering over a window will cause a selection rectangle to appear over it.
* Clicking on a window makes slop return the dimensions of the window, and it's ID.
* OpenGL accelerated graphics where possible.
* Supports simple arguments:
    * Change selection rectangle border size.
    * Select X display.
    * Set padding size.
    * Force window, or pixel selections with the tolerance flag.
    * Set the color of the selection rectangles to match your theme! (Even supports transparency!)
    * Remove window decorations from selections.
* Supports custom programmable shaders.

## Practical Applications
slop can be used to create a video recording script in only two lines of code.
```bash
#!/bin/bash
read -r X Y W H G ID < <(slop -f "%x %y %w %h %g %i")
ffmpeg -f x11grab -s "$W"x"$H" -i :0.0+$X,$Y -f alsa -i pulse ~/myfile.webm
```

You can also take images using imagemagick like so:
```bash
#!/bin/bash
read -r G < <(slop -f "%g")
import -window root -crop $G ~/myimage.png
```
If you don't like ImageMagick's import: Check out [maim](https://github.com/naelstrof/maim) for a better screenshot utility.

## Lets see some action
Ok. Here's a comparison between 'scrot -s's selection and slop's:

![scrotbad](http://farmpolice.com/content/images/2014-10-14-12:08:24.png)
![slopgood](http://farmpolice.com/content/images/2014-10-14-12:14:51.png)

You can see scrot leaves garbage lines over the things you're trying to screenshot!
While slop not only looks nicer, it's impossible for it to end up in screenshots or recordings because it waits for DestroyNotify events before completely shutting down. Only after the window is completely destroyed can anything take a screenshot.

## how to install

### Install using your Package Manager (Preferred)

* [Arch Linux: community/slop](https://www.archlinux.org/packages/community/x86_64/slop/)
* [Void Linux: slop](https://github.com/voidlinux/void-packages/blob/24ac22af44018e2598047e5ef7fd3522efa79db5/srcpkgs/slop/template)
* [FreeBSD: x11/slop](http://www.freshports.org/x11/slop/)
* [OpenBSD: graphics/slop](http://openports.se/graphics/slop)
* [CRUX: 6c37/slop](https://github.com/6c37/crux-ports/tree/3.2/slop)
* [Gentoo: x11-misc/slop](https://packages.gentoo.org/packages/x11-misc/slop)
* [GNU Guix: slop](https://www.gnu.org/software/guix/packages/#slop)
* [Debian: slop](https://packages.debian.org/sid/slop)
* Please make a package for slop on your favorite system, and make a pull request to add it to this list.

### Install using CMake (Requires CMake)

*Note: Dependencies should be installed first: libxext, OpenGL, and glm.*

```bash
git clone https://github.com/naelstrof/slop.git
cd slop
cmake -DCMAKE_INSTALL_PREFIX="/usr" ./
make && sudo make install
```

### Shaders
These are implemented, but not flushed out. I haven't gotten around to writing some tutorials on them. So check back later!
