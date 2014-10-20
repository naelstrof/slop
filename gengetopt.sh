#!/bin/bash
# This is used because a bug in gengetopt keeps you from putting \n in default string options. All it does is replace REPLACEME with the correct code to make the application work properly.
gengetopt < options.ggo
sed -i '0,/REPLACEME/{s/REPLACEME/X=%x\\\\nY=%y\\\\nW=%w\\\\nH=%h\\\\nG=%g\\\\nID=%i\\\\nCancel=%c\\\\n/}' cmdline.c
sed -i 's/REPLACEME/X=%x\\nY=%y\\nW=%w\\nH=%h\\nG=%g\\nID=%i\\nCancel=%c\\n/' cmdline.c
