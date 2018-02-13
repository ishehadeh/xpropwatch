# xpropwatch

A utility for watching X Window Properties, written in C++ with XCB.

xpropwatch sleeps until a specific x11 window property changes, then prints it, formatted as JSON.

The property types that xpropwatch currently supports are:

| Property Type | Output Type                                                                                |
| ------------- | ------------------------------------------------------------------------------------------ |
| CARDINAL      | number                                                                                     |
| WINDOW        | ID (number)                                                                                |
| BITMAP        | ID                                                                                         |
| COLORMAP      | ID                                                                                         |
| CURSOR        | ID                                                                                         |
| DRAWABLE      | ID                                                                                         |
| FONT          | ID                                                                                         |
| VISUALID      | ID                                                                                         |
| INTGER        | number                                                                                     |
| ATOM          | quoted string                                                                              |
| ARC           | object: {"x": number, "y": number, "angles": number[2], "width": number, "height": number} |
| POINT         | object: {"x": number, "y": number}                                                         |
| RECTANGLE     | object: {"x": number, "y": number, "width": number, "height": number}                      |


## Building

1. `git clone https://github.com/IanS5/xpropwatch`
2. `cd xpropwatch`
3. `mkdir build && cd build`
4. `cmake ..`
5. `cmake --build .`
## Usage

`./xpropwatch [options] <prop-name>`

### OPTIONS:

- -h: display this message and exit
- -i: print the property's value immediately
- -k: exit as soon as the property is printed (from an event or -i)
- -w <window>: watch properties from <window> (default: ROOT)
