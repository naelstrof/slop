slrn
----

slrn is an application that literally just allows a user to select a region of his screen. It grabs the cursor, draws a fancy box, and prints the selection's dimensions and position to stdout.

It does a better job than scrot's selection since it doesn't draw directly to the root window, it creates its own to draw the selection rectangle.
However due to the X11's Shape extension and window resizing not being synchronized it doesn't render a perfect box during movement. I'm hoping to fix this later by never resizing the window.
