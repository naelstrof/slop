slrn
====

slrn is an application that literally just allows a user to select a region of his screen. It grabs the cursor, draws a fancy box, and prints the selection's dimensions and position to stdout.

features
--------
* Clicking on a window makes slrn return the dimensions of the window.
* Clicking and dragging causes a selection rectangle to appear, renders pretty well (compared to scrot). And will return the dimensions of that rectangle in absolute screen coords.
* Hovering over a window will cause a selection rectangle to appear over it.
* On startup it turns your cursor into a crosshair, then adjusts the cursor into angles as you drag the selection rectangle.
