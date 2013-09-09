slrn
====

slrn (Select Region) is an application that querys for a selection from the user and prints the region to stdout. It grabs the mouse and turns it into a crosshair, lets the user click and drag to make a selection (or click on a window) while drawing a pretty box around it, then finally prints the selection's dimensions to stdout.

features
--------
* Hovering over a window will cause a selection rectangle to appear over it.
* Clicking on a window makes slrn return the dimensions of the window.
* Clicking and dragging causes a selection rectangle to appear, renders pretty well (compared to scrot). And will return the dimensions of that rectangle in absolute screen coords.
* On startup it turns your cursor into a crosshair, then adjusts the cursor into angles as you drag the selection rectangle.
* Supports simple arguments (change selection rectangle border size, x display, padding, click tolerance, etc. )

practical applications
----------------------
slrn can be used to create a video recording script in only two lines of code.
```bash
#!/bin/bash
eval `slrn`
ffmpeg -f x11grab -s "$Wx$H" -i :0.0+$X,$Y -f alsa -i pulse ~/myfile.webm
```
slrn lets you select a region or window and ffmpeg will record it in the format of your choice!
Combined with keybinds and a server on your filesystem you can make a really neat and unobtrusive screenshooter.
