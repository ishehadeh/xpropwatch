# xpropwatch


A utility for watching X Window Properties, written in C++ with XCB.

xpropwatch prints properties as json. Many properties aren't actual values however, just IDs.
For instance an bitmap will be represented as an integer, even though it's not.
In most cases these IDs are better, becuase the actual structure is large, and could take a while to fetch
Here is all the property types xpropwatch supports and how they are represented when printed:

| Property  | Representation                                                                                                                            |
|-----------|-------------------------------------------------------------------------------------------------------------------------------------------|
| CARDINAL  | an unsigned integer, e.g. 5                                                                                                               |
| WINDOW    | the window's id, an unsigned integer, e.g. 5 (an option to print the name as a string is coming soon!)                                    |
| BITMAP    | the bitmap's id, an unsigned integer, e.g 5                                                                                               |
| COLORMAP  | the colormap's id, an unsigned integer, e.g 5                                                                                             |
| CURSOR    | the cursor's id, an unsigned integer, e.g 5                                                                                               |
| DRAWABLE  | the drawable's id, an unsigned integer, e.g. 5                                                                                            |
| FONT      | the font's id, an unsigned integer, e.g. 5                                                                                                |
| VISUALID  | an unsigned integer e.g. 5                                                                                                                |
| INTGER    | an integer, e.g. -10                                                                                                                      |
| ATOM      | the atom's name a string, e.g. "WM_NAME"                                                                                                  |
| ARC       | the arc's actual structure, an object in the form of {"x": <integer>, "y": integer, "angles": [2]integer, "width": uint, "height":"uint"} |
| POINT     | the point's actual structure, an object in the form of {"x": integer, "y": integer}                                                       |
| RECTANGLE | the rectangle's actual structure, an object in the form of {"x": integer, "y": integer, "width": uint, "height": uint}                    |
## Building

To build xpropwatch:

1. `git clone https://github.com/IanS5/xpropwatch`
2. `cd xpropwatch`
3. `make`

Now there should get an executable call `xpropwatch`

## Usage

The command line format is `xpropwatch [options] <prop>`
By defualt xpropwatch watches a property on the root window, for example
`xpropwatch _NET_CURRENT_DESKTOP` will begin watching the `_NET_CURRENT_DESKTOP` 
property of the root window.

Right now there are only a few flags, more will be coming soon!

- `-h` help message
- `-w <window>` watch a property on a window, the window's name just needs to include `<window>` in it's name to qualify
- `-i` print out the property immediately, instead of waiting for the first change
- `-k` exit as soon as the property is printed

