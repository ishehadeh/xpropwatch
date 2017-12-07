#include "core.h"

std::shared_ptr<Window> Screen::WindowByName(std::string &name) {
    std::shared_ptr<Window> root = std::shared_ptr<Window>(new Window(conn, screen->root));
    if(name == "ROOT") {
        return root;
    }
    std::shared_ptr<Window> win = WindowByNameRecursive(root, name);
    if(win == nullptr) {
        throw ErrNoWindow(name);
    }
    return win;
}

Atom Connection::InternAtom(std::string name) {
    xcb_generic_error_t* err = NULL;
    xcb_intern_atom_reply_t* repl = xcb_intern_atom_reply(conn, xcb_intern_atom(conn, 0, name.size(), name.c_str()), &err);
    if(err != NULL) {
        throw ErrXError("failed to intern atom " + name, err);
    }
    xcb_atom_t ret = repl->atom;
    free(repl);
    return ret;
}

Screen::Screen(const Connection &c, int s) : conn(c) {
    xcb_screen_iterator_t iter;
    iter = xcb_setup_roots_iterator (xcb_get_setup (c.Raw()));
    for (; iter.rem; --s, xcb_screen_next (&iter)) {
        if (s == 0) {
            screen = iter.data;
            return;
        }
    }
    throw ErrNoScreen(s);
}

std::string Window::Name() {
    xcb_generic_error_t* err;
    auto cookie = xcb_get_property(conn.Raw(), 0, window, XCB_ATOM_WM_NAME, XCB_GET_PROPERTY_TYPE_ANY, 0, 100);
    xcb_get_property_reply_t* repl = xcb_get_property_reply(conn.Raw(), cookie, &err);
    if(err != NULL) {
        free(repl);
        throw ErrXError("failed to get window name", err);
    }
    if(repl->length == 0) {
        free(repl);
        return "";
    }
    int len = xcb_get_property_value_length(repl);
    std::string ret((char*)xcb_get_property_value(repl), len);
    free(repl);
    return ret;
}

std::shared_ptr<Window> Screen::WindowByNameRecursive(std::shared_ptr<Window> win, std::string &name) {
    if(win->Name().find(name) != std::string::npos) {
        return win;
    }
    xcb_generic_error_t* err = NULL;
    xcb_query_tree_reply_t* repl = xcb_query_tree_reply(conn.Raw(), xcb_query_tree(conn.Raw(), win->Raw()), &err);
    if(err != NULL) {
        throw ErrXError("while searching for window " + name + ": failed to get tree", err);
        return nullptr;
    }
    xcb_window_t* children = xcb_query_tree_children(repl);
    for(int i = 0; i < repl->children_len; ++i) {
        std::shared_ptr<Window> recW = WindowByNameRecursive(std::shared_ptr<Window>(new Window(conn, children[i])), name);
        if (recW != nullptr) {
            free(repl);
            return recW;
        }
    }
    free(repl);
    return nullptr;
}

uint8_t Window::PollEvent() {
    ev = xcb_wait_for_event(conn.Raw());
    return ev->response_type;
}

uint8_t Window::WaitEvent() {
    ev = xcb_wait_for_event(conn.Raw());
    return ev->response_type;
}

void Window::ListenForEvents(std::vector<uint32_t> masks) {
    xcb_change_window_attributes(conn.Raw(), window, XCB_CW_EVENT_MASK, masks.data());
    xcb_flush(conn.Raw());
}

std::unique_ptr<xcb_get_property_reply_t, decltype(free)*> Window::GetProp(Atom prop) {
    xcb_generic_error_t* err = NULL;
    xcb_get_property_cookie_t cookie = xcb_get_property(conn.Raw(), 0, window, prop, XCB_GET_PROPERTY_TYPE_ANY, 0, 100);
    xcb_get_property_reply_t* repl = xcb_get_property_reply(conn.Raw(), cookie, &err);
    if(err != NULL) {
        throw ErrXError("error finding prop", err);
    }
    return std::unique_ptr<xcb_get_property_reply_t, decltype(free)*>(repl, free);
}

Screen Connection::GetScreen(int screen) {
    if(screen == -1) {
        screen = preferedScreen;
    }
    return Screen(*this, screen);
}

std::string Connection::AtomName(Atom a){
    xcb_generic_error_t* err = NULL;
    xcb_get_atom_name_cookie_t name_cookie = xcb_get_atom_name(conn, a);
    xcb_get_atom_name_reply_t* name_reply = xcb_get_atom_name_reply(conn, name_cookie, &err);
    if(err != NULL) {
        throw ErrXError("error finding prop", err);
    }
    std::string ret = xcb_get_atom_name_name(name_reply);
    free(name_reply);
    return ret;
}