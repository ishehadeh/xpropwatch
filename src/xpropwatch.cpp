#include "core.h"


void print_prop(Connection &conn, std::shared_ptr<Window> window, Atom prop) {
    auto repl = window->GetProp(prop);
    void* val = xcb_get_property_value(repl.get());
    if(repl->format == 0) {
        printf("\n");
        return;
    }
    int fmt_bytes = repl->format/8;

    int len = xcb_get_property_value_length(repl.get())/fmt_bytes;
    if(len > 1 && repl->type != XCB_ATOM_STRING) {
        printf("[");
    }
    switch(repl->type) {
        case XCB_ATOM_CARDINAL:
        case XCB_ATOM_WINDOW: // TODO print the window name 
        case XCB_ATOM_BITMAP:
        case XCB_ATOM_COLORMAP:
        case XCB_ATOM_CURSOR:
        case XCB_ATOM_DRAWABLE:
        case XCB_ATOM_FONT:
        case XCB_ATOM_VISUALID:
        {
            XIdent* ident_arr = (XIdent*)val;
            for(int i = 0; i < len; ++i) {
                if (i != len-1) {
                    printf("%u,", ident_arr[i]);
                }else{
                    printf("%u", ident_arr[i]);
                }
            }
            break;
        }
        case XCB_ATOM_INTEGER:
        {
            int32_t* ident_arr = (int32_t*)val;
            for(int i = 0; i < len; ++i) {
                if (i != len-1) {
                    printf("%d,", ident_arr[i]);
                }else{
                    printf("%d", ident_arr[i]);
                }
            }
            break;
        }
	case XCB_ATOM_ATOM:
	{
	    xcb_atom_t* atom_arr = (xcb_atom_t*)val;
            for(int i =- 0; i < len; ++i) {
	        if(i != len-1) {
		    printf("\"%s\",", conn.AtomName(atom_arr[i]).c_str());
		}else{
		    printf("\"%s\"", conn.AtomName(atom_arr[i]).c_str());
		}
	    }
	    break;
	}
        case XCB_ATOM_STRING:
        {
            fwrite("\"", 1, 1, stdout);
            fwrite(val, len, 1, stdout);
            fwrite("\"", 1, 1, stdout);
            break;
        }
        case XCB_ATOM_ARC:
        {
            xcb_arc_t* arc_arr = (xcb_arc_t*)val;
            for(int i = 0; i < len; ++i) {
                if (i != len-1) {
                    printf("{\"x\":%d,\"y\":%d,\"angles\":[%d,%d],},\"width\":%u\"height\":%u}",
                        arc_arr[i].x, arc_arr[i].y, arc_arr[i].angle1, arc_arr[i].angle2, arc_arr[i].width, arc_arr[i].height);
                }else{
                    printf("{\"x\":%d,\"y\":%d,\"angles\":[%d,%d],},\"width\":%u\"height\":%u},",
                        arc_arr[i].x, arc_arr[i].y, arc_arr[i].angle1, arc_arr[i].angle2, arc_arr[i].width, arc_arr[i].height);
                }
            }
            break;
        }
        case XCB_ATOM_POINT:
        {
            xcb_point_t* pt_arr = (xcb_point_t*)val;
            for(int i = 0; i < len; ++i) {
                if (i != len-1) {
                    printf("{\"x\":%d,\"y\":%d}", pt_arr[i].x, pt_arr[i].y);
                }else{
                    printf("{\"x\":%d,\"y\":%d},", pt_arr[i].x, pt_arr[i].y);
                }
            }
            break;
        }
        case XCB_ATOM_RECTANGLE:
        {
            xcb_rectangle_t* rect_arr = (xcb_rectangle_t*)val;
            for(int i = 0; i < len; ++i) {
                if (i != len-1) {
                    printf("{\"x\":%d,\"y\":%d,\"width\":%u\"height\":%u}", rect_arr[i].x, rect_arr[i].y, rect_arr[i].width, rect_arr[i].height);
                }else{
                    printf("{\"x\":%d,\"y\":%d, \"width\":%u\"height\":%u},", rect_arr[i].x, rect_arr[i].y, rect_arr[i].width, rect_arr[i].height);
                }
            }
            break;
        }

    }
    if(len > 1 && repl->type != XCB_ATOM_STRING) {
        printf("]\n");
    }else{
        printf("\n");
    }
}

void usage(char* name) {
    printf("USAGE: %s [options] <prop-name>\n", name);
    printf("OPTIONS:\n");
    printf("\t-h: display this message and exit\n");
    printf("\t-i: print the property's value immediately\n");
    printf("\t-k: exit as soon as the property is printed (from an event or -i)\n");
    printf("\t-w <window>: watch properties from <window> (default: ROOT)\n");
}
int main(int argc, char** argv) {
    std::string window_name = "ROOT";
    bool        kill = false;
    bool        alive = true;
    int         initial = false;

    int c = 0;  
    while((c = getopt(argc, argv, "hkiw:")) != -1) {
        switch(c) {
            case 'w': 
                window_name = optarg;
                break;
            case 'k':
                kill = true;
                break;
            case 'i':
                initial = true;
                break;
            case 'h':
                usage(argv[0]);
                exit(0);
            default:
                printf("unkown option '%c'\n-h to display usage\n", c);
                exit(1);
        }

    }
    if(argc <= optind) {
        usage(argv[0]);
        exit(1);
    }
    Connection conn;
    Atom target = conn.InternAtom(argv[optind]);
    Screen screen = conn.GetScreen();
    std::shared_ptr<Window> window = screen.WindowByName(window_name);
    if (initial) {
        print_prop(conn, window, target);
        if(kill) alive = false;
    }
    window->ListenForEvents({XCB_EVENT_MASK_PROPERTY_CHANGE});

    while(alive) {
        uint8_t ev = window->WaitEvent();
        switch(ev) {
            case XCB_PROPERTY_NOTIFY: 
            {
                Atom a = window->Event<xcb_property_notify_event_t>()->atom;
                if (a == target) {
                    print_prop(conn, window, a);
                    if(kill) alive = false;
                }
            }
        }
    }
}
