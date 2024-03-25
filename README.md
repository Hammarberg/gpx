A simple command line utility to create gpx files from a collection of map, scr and col files.

The tool only works with standard 40*25 cell buffers of 8000 bytes for the map and 1000 bytes for each scr and col.

Output is send to stdout.

Usage:
gpx <screens wide> <screens high> <map, col & screen files> > <output>.gpx

Input files are expected in the order from top left to bottom right.

Example:
gpx 2 1 one.map one.scr one.col two.map two.scr two.col > out.gpx
