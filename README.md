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
slop=$(slop -f "%x %y %w %h %g %i") || exit 1
read -r X Y W H G ID < <(echo $slop)
ffmpeg -f x11grab -s "$W"x"$H" -i :0.0+$X,$Y -f alsa -i pulse ~/myfile.webm
```

You can also take images using imagemagick like so:
```bash
#!/bin/bash
slop=$(slop -f "%g") || exit 1
read -r G < <(echo $slop)
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
* [NetBSD: x11/slop](http://pkgsrc.se/x11/slop)
* [OpenBSD: graphics/slop](http://openports.se/graphics/slop)
* [CRUX: 6c37/slop](https://github.com/6c37/crux-ports/tree/3.2/slop)
* [Gentoo: x11-misc/slop](https://packages.gentoo.org/packages/x11-misc/slop)
* [GNU Guix: slop](https://www.gnu.org/software/guix/packages/#slop)
* [Debian: slop](https://packages.debian.org/sid/slop)
* [Fedora: slop](https://src.fedoraproject.org/rpms/slop)
* Please make a package for slop on your favorite system, and make a pull request to add it to this list.

### Install using CMake (Requires CMake)

*Note: Dependencies should be installed first: libxext, glew, and glm.*

```bash
git clone https://github.com/naelstrof/slop.git
cd slop
cmake -DCMAKE_INSTALL_PREFIX="/usr" ./
make && sudo make install
```

### Shaders

Slop allows for chained post-processing shaders. Shaders are written in a language called GLSL, and have access to the following data from slop:

| GLSL Name  | Data Type      | Bound to                                                                                        |
|------------|----------------|-------------------------------------------------------------------------------------------------|
| mouse      | vec2           | The mouse position on the screen.                                                               |
| desktop    | sampler2D      | An upside-down snapshot of the desktop, this doesn't update as the screen changes.              |
| texture    | sampler2D      | The current pixel values of slop's frame buffer. Usually just contains the selection rectangle. |
| screenSize | vec2           | The dimensions of the screen, where the x value is the width.                                   |
| position   | vec2 attribute | This contains the vertex data for the rectangle. Only contains (0,0), (1,0), (1,1), and (0,1).  |
| uv         | vec2 attribute | Same as the position, this contians the UV information of each vertex.                          |

The desktop texture is upside-down because flipping it would cost valuable time.

Shaders must be placed in your `${XDG_CONFIG_HOME}/slop` directory, where *XDG_CONFIG_HOME* is typically `~/.config/`. This folder won't exist unless you make it yourself.

Shaders are loaded from the `--shader` flag in slop. They are delimited by commas, and rendered in order from left to right. This way you can combine multiple shaders for interesting effects! For example, `slop -rblur1,wiggle` would load `~/.config/slop/blur1{.frag,.vert}` and `~/.config/slop/wiggle{.frag,.vert}`. Then render the selection rectangle twice, each time accumulating the changes from the different shaders.

Enough chatting about it though, here's some example shaders you can copy from [shaderexamples](https://github.com/naelstrof/slop/tree/master/shaderexamples) to `~/.config/slop` to try out!

The files listed to the right of the `|` are the required files for the command to the left to work correctly.
* `slop -r blur1,blur2 -b 100` | `~/.config/slop/{blur1,blur2}{.frag,.vert}`

![slop blur](https://my.mixtape.moe/bvsrzr.png)

* `slop -r wiggle -b 10` | `~/.config/slop/wiggle{.frag,.vert}`

![slop animation](http://i.giphy.com/12vjSbFZ0CWDW8.gif)

And all together now...
* `slop -r blur1,blur2,wiggle -b 50 -c 1,1,1` | `~/.config/slop/{blur1,blur2,wiggle}{.frag,.vert}`

![slop animation](http://i.giphy.com/kfBLafeJfLs2Y.gif)

Finally here's an example of a magnifying glass.
* `slop -r crosshair` | `~/.config/slop/crosshair{.frag,.vert}`

![slop animation](http://i.giphy.com/2xy0fC2LOFQfm.gif)

It's fairly easy to adjust how the shaders work by editing them with your favourite text editor. Or even make your own!
