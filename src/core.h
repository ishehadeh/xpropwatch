#include <xcb/xcb.h>

#include <exception>
#include <string>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <vector>

class Connection;
class Screen;
class Window;
typedef uint32_t XIdent;
typedef XIdent Atom;

class ErrNoScreen : std::exception {
    std::string msg;
public:
    explicit ErrNoScreen(int screen) {
        msg = "failed to find screen " + std::to_string(screen);
    }
    virtual const char* what() const throw () {
        return msg.c_str();
    }
};

class ErrNoWindow : std::exception {
    std::string msg;
public:
    explicit ErrNoWindow(std::string window) {
        msg = "failed to find window " + window;
    }
    virtual const char* what() const throw () {
        return msg.c_str();
    }  
};

class ErrXError : std::exception {
    std::string msg;
public:
    explicit ErrXError(std::string explainer, xcb_generic_error_t *err) {
        msg = explainer + "; X11 Error: " + std::to_string(err->error_code);
        free(err);
    }
    virtual const char* what() const throw () {
        return msg.c_str();
    }
};

class Window {
private:
    xcb_window_t window;
    const Connection   &conn;
    xcb_generic_event_t* ev;
public:
    Window(const Connection &conn, xcb_window_t id) : conn(conn), window(id) {};

    inline xcb_window_t Raw() const { return window; }
    std::string Name();

    void ListenForEvents(std::vector<uint32_t> masks);

    // wait and poll event return the event's type the actual event can be fetched with Window::Event()
    uint8_t WaitEvent();
    uint8_t PollEvent();

    template<typename T = xcb_generic_event_t>
    std::unique_ptr<T, decltype(free)*> Event() {
        return std::unique_ptr<T, decltype(free)*>((T*)ev, free);
    }
    std::unique_ptr<xcb_get_property_reply_t, decltype(free)*> GetProp(Atom prop);
};


class Screen {
private:
    xcb_screen_t*       screen;
    const Connection    &conn;
    std::shared_ptr<Window> WindowByNameRecursive(std::shared_ptr<Window> win, std::string &name);
public:
    Screen(const Connection &c, int s);
    std::shared_ptr<Window> WindowByName(std::string &name);
};

class Connection {
private:
    xcb_connection_t* conn;
    int preferedScreen;
public:
    Connection(const char* display = NULL) {
        conn = xcb_connect(display, &preferedScreen);
    }
    xcb_connection_t* Raw() const {
        return conn;
    }
    Screen GetScreen(int screen = -1);
    Atom InternAtom(std::string name);
    std::string AtomName(Atom a);
    ~Connection() {
        xcb_disconnect(conn);
    }
};
