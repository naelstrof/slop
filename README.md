slrn
====

slrn (Select Region) is an application that querys for a selection from the user and prints the region to stdout. It grabs the mouse and turns it into a crosshair, lets the user click and drag to make a selection (or click on a window) while drawing a pretty box around it, then finally prints the selection's dimensions to stdout.

features
--------
* Hovering over a window will cause a selection rectangle to appear over it.
* Clicking on a window makes slrn return the dimensions of the window.
* Clicking and dragging causes a selection rectangle to appear, renders pretty well (compared to scrot). And will return the dimensions of that rectangle in absolute screen coords.
* On startup it turns your cursor into a crosshair, then adjusts the cursor into angles as you drag the selection rectangle.
* Supports simple arguments:
    * Change selection rectangle border size.
    * Select X display.
    * Set padding size, even to negative padding sizes!
    * Set click tolerance for if you have a shaky mouse.
    * Set the color of the selection rectangles to match your theme!

practical applications
----------------------
slrn can be used to create a video recording script in only two lines of code.
```bash
#!/bin/bash
eval `slrn`
ffmpeg -f x11grab -s "$W"x"$H" -i :0.0+$X,$Y -f alsa -i pulse ~/myfile.webm
```
slrn lets you select a region or window and ffmpeg will record it in the format of your choice!
Combined with keybinds and a server on your filesystem you can make a really neat and unobtrusive screenshooter.

You can also take images using imagemagick like so:
```bash
#!/bin/bash
eval `slrn`
import -window root -crop "$W"x"$H"+$X+$Y ~/myimage.png
```

lets see some action
--------------------
Ok. Here's a comparison between 'scrot -s's selection and slrn's:
![scrot](http://farmpolice.com/content/images/scrot_bad.png)
![slrn](http://farmpolice.com/content/images/slrn_good.png)

You can see scrot leaves garbage lines over the things you're trying to screenshot!
While slrn not only looks nicer, it's impossible for it to end up in screenshots or recordings because it shuts down before ffmpeg or imagemagick can take a picture.
