slop
====

slop (Select Operation) is an application that querys for a selection from the user and prints the region to stdout. It grabs the mouse and turns it into a crosshair, lets the user click and drag to make a selection (or click on a window) while drawing a pretty box around it, then finally prints the selection's dimensions to stdout.

features
--------
* Hovering over a window will cause a selection rectangle to appear over it.
* Clicking on a window makes slop return the dimensions of the window.
* Clicking and dragging causes a selection rectangle to appear, renders pretty well (compared to scrot). And will return the dimensions of that rectangle in absolute screen coords.
* On startup it turns your cursor into a crosshair, then adjusts the cursor into angles as you drag the selection rectangle.
* Supports simple arguments:
    * Change selection rectangle border size.
    * Select X display.
    * Set padding size, even negative padding sizes!
    * Set click tolerance for if you have a shaky mouse.
    * Set the color of the selection rectangles to match your theme! (Even supports transparency!)
    * Remove window decorations from selections.

practical applications
----------------------
slop can be used to create a video recording script in only two lines of code.
```bash
#!/bin/bash
eval `slop`
ffmpeg -f x11grab -s "$W"x"$H" -i :0.0+$X,$Y -f alsa -i pulse ~/myfile.webm
```
slop lets you select a region or window and ffmpeg will record it in the format of your choice!
Combined with keybinds and a server on your filesystem you can make a really neat and unobtrusive screenshooter.

You can also take images using imagemagick like so:
```bash
#!/bin/bash
eval `slop`
import -window root -crop $G ~/myimage.png
```
If you don't like ImageMagick's import: Check out [maim](https://github.com/naelstrof/maim) for a better screenshot utility.

You can see my implementation of slop in a screenshooter here:
https://gist.github.com/naelstrof/6530959

lets see some action
--------------------
Ok. Here's a comparison between 'scrot -s's selection and slop's:
![scrotbad](http://farmpolice.com/content/images/2014-10-14-12:08:24.png)
![slopgood](http://farmpolice.com/content/images/2014-10-14-12:14:51.png)

You can see scrot leaves garbage lines over the things you're trying to screenshot!
While slop not only looks nicer, it's impossible for it to end up in screenshots or recordings because it waits for DestroyNotify events before completely shutting down. Only after the window is completely destroyed can anything take a screenshot.

how to install
--------------
You really should use your package manager when possible, you never know what kind of garbage applications will dump everywhere. For example you can install slop on Arch Linux by installing it from the AUR: [slop-git](https://aur.archlinux.org/packages/slop-git/).

However slop just installs a single binary to /usr/bin. If that's ok with you-- you can install it with:
```bash
make && sudo make install
```
Make sure to check out and install [maim](https://github.com/naelstrof/maim) too if you want a proper screenshot utility.

help
----
```text
slop v3.1.3

Copyright (C) 2014 Dalton Nell, GPLv3

Usage: slop [options]

slop (Select Operation) is an application that queries for a selection from the
user and prints the region to stdout.

  -h, --help                    Print help and exit
  -V, --version                 Print version and exit
Options
      --xdisplay=hostname:number.screen_number
                                Sets the x display.  (default=`:0')
      --nokeyboard              Disables the ability to cancel selections with
                                  the keyboard.  (default=off)
  -b, --bordersize=INT          Set the selection rectangle's thickness. Does
                                  nothing when --highlight is enabled.
                                  (default=`5')
  -p, --padding=INT             Set the padding size of the selection. Can be
                                  negative.  (default=`0')
  -t, --tolerance=INT           How far in pixels the mouse can move after
                                  clicking and still be detected as a normal
                                  click instead of a click and drag. Setting
                                  this to 0 will disable window selections.
                                  (default=`2')
  -g, --gracetime=FLOAT         Set the amount of time before slop will check
                                  for keyboard cancellations in seconds.
                                  (default=`0.4')
  -c, --color=FLOAT,FLOAT,FLOAT,FLOAT
                                Set the selection rectangle's color. Supports
                                  RGB or RGBA values.
                                  (default=`0.5,0.5,0.5,1')
  -n, --nodecorations           Attempt to select child windows in order to
                                  avoid window decorations.  (default=off)
      --min=INT                 Set the minimum output of width or height
                                  values. This is useful to avoid outputting 0.
                                  Setting min and max to the same value
                                  disables drag selections.  (default=`0')
      --max=INT                 Set the maximum output of width or height
                                  values. Setting min and max to the same value
                                  disables drag selections.  (default=`0')
  -l, --highlight               Instead of outlining selections, slop
                                  highlights it. This is only useful when
                                  --color is set to a transparent color.
                                  (default=off)

Examples
    $ # Gray, thick, transparent border for maximum visiblity.
    $ slop -b 20 -c 0.5,0.5,0.5,0.8

    $ # Remove window decorations.
    $ slop --nodecorations

    $ # Disable window selections. Useful for selecting individual pixels.
    $ slop -t 0

    $ # Classic Windows XP selection.
    $ slop -l -c 0.3,0.4,0.6,0.4
```
