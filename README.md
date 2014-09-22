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
    * Set the color of the selection rectangles to match your theme!
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
import -window root -crop "$W"x"$H"+$X+$Y ~/myimage.png
```

You can see my implementation of slop in a screenshooter here:
https://gist.github.com/naelstrof/6530959

lets see some action
--------------------
Ok. Here's a comparison between 'scrot -s's selection and slop's:
![scrotbad](http://farmpolice.com/content/images/scrot_bad.png)
![slopgood](http://farmpolice.com/content/images/slrn_good.png)

You can see scrot leaves garbage lines over the things you're trying to screenshot!
While slop not only looks nicer, it's impossible for it to end up in screenshots or recordings because it waits for DestroyNotify events before completely shutting down. Only after the window is completely destroyed can ffmpeg or imagemagick take a picture.

help
----
```text
Usage: slop [options]
Print user selected region to stdout. Pressing keys or right-clicking cancels selection.

Options
    -h, --help                     Show this message.
    -nkb, --nokeyboard             Disables the ability to cancel selections with the keyboard.
    -b=INT, --bordersize=INT       Set selection rectangle border size.
    -p=INT, --padding=INT          Set padding size for selection.
    -t=INT, --tolerance=INT        How far in pixels the mouse can move after clicking and still be detected
                                   as a normal click. Setting to zero will disable window selections.
    -x=STRING, --xdisplay=STRING   Set x display (STRING must be hostname:number.screen_number format)
    -c=COLOR, --color=COLOR        Set selection rectangle color, COLOR is in format FLOAT,FLOAT,FLOAT
    -g=FLOAT, --gracetime=FLOAT    Set the amount of time before slop will check for keyboard cancellations
                                   in seconds.
    -nd, --nodecorations           attempts to remove decorations from window selections.
    -o=GEOMETRY, --offset=GEOMETRY Offsets window selections, but only if --nodecorations is active and if the
                                   window's decorations were successfully detected. Has a very specific use of
                                   removing shadows from Gnome's window selections right now. GEOMETRY is in
                                   format WxH+X+Y

Examples
    $ # gray, thick border for maximum visiblity.
    $ slop -b=20 -c=0.5,0.5,0.5

    $ # Remove window decorations, but include the 28px titlebar. Useful to remove the arbitrarily sized shadows in Gnome where they are included in window geometry for whatever reason.
    $ slop -nd -o=0x28+0-28

    $ # Disable window selections. Useful for selecting individual pixels.
    $ slop -t=0
```
