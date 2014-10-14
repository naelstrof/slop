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
    -c=COLOR, --color=COLOR        Set selection rectangle color, COLOR is in format FLOAT,FLOAT,FLOAT,FLOAT.
                                   takes RGBA or RGB.
    -g=FLOAT, --gracetime=FLOAT    Set the amount of time before slop will check for keyboard cancellations
                                   in seconds.
    -nd, --nodecorations           Attempts to remove decorations from window selections.
    -min=INT, --minimumsize=INT    Sets the minimum output of width or height values, useful to avoid outputting 0
                                   widths or heights.
    -max=INT, --maximumsize=INT    Sets the maximum output of width or height values.
    -hi, --highlight               Instead of outlining the selection, slop highlights it. Only useful when
                                   used with a --color with an alpha under 1.
    -v, --version                  prints version.

Examples
    $ # Gray, thick, transparent border for maximum visiblity.
    $ slop -b=20 -c=0.5,0.5,0.5,0.8

    $ # Remove window decorations.
    $ slop -nd

    $ # Disable window selections. Useful for selecting individual pixels.
    $ slop -t=0

    $ # Classic Windows XP selection.
    $ slop -hi -c=0.2,0.4,0.5,0.4
```
