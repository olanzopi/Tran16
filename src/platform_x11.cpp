#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <algorithm>

#include <platform.h>

struct window_t {
    Display* display;
    Window   win;
    GC gc;
    XImage* image;
    uint32_t* pixel_buffer;
    uint16_t width, height;
    Atom wm_delete_window;

    bool keys[65536] = {false};
    int mouse_x, mouse_y;
    bool mouse_buttons[5];
};

window_t* create_window(std::string name, uint16_t pos_x, uint16_t pos_y, uint16_t size_x, uint16_t size_y, uint32_t* pixel_buffer) {
    Display* display = XOpenDisplay(nullptr);

    if (display == nullptr) {
        fprintf(stderr, "XOpenDisplay failed — couldn't connect to X server\n");
        return nullptr;
    }

    int screen = DefaultScreen(display);

    Window win = XCreateSimpleWindow(
        display, 
        RootWindow(display, DefaultScreen(display)),
        pos_x, pos_y,
        size_x, size_y,
        0,0,0
    );

    XStoreName(display, win, name.c_str());
    XMapWindow(display, win);

    XSelectInput(display, win,
        ExposureMask | KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask
    );

    XFlush(display);

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, win, &wm_delete_window, 1);

    GC gc = XCreateGC(display, win, 0, nullptr);

    XImage* image = XCreateImage(
        display,
        DefaultVisual(display, screen),
        24,
        ZPixmap, 0,
        (char*)pixel_buffer,
        size_x, size_y,
        32,
        0
    );

    window_t* window = new window_t;
    window->win = win;
    window->display = display;
    window->gc = gc;
    window->image = image;
    window->pixel_buffer = pixel_buffer;
    window->width = size_x;
    window->height = size_y;
    window->wm_delete_window = wm_delete_window; 
    return window;
}

void draw(window_t* window) {
    XPutImage(
        window->display, window->win, window->gc, window->image,
        0, 0, 0, 0,
        window->width, window->height
    );
    XFlush(window->display);
}

bool poll_events(window_t* window) {
    while (XPending(window->display) > 0) {
        XEvent event;
        XNextEvent(window->display, &event);

        switch (event.type)
        {
            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == window->wm_delete_window) {
                    return false;
                }
                break;
            case KeyPress: {
                KeySym sym = XLookupKeysym(&event.xkey, 0);
                if (sym < 65536)
                    window->keys[sym] = true;
                break;
            }

            case KeyRelease: {
                KeySym sym = XLookupKeysym(&event.xkey, 0);
                if (sym < 65536)
                    window->keys[sym] = false;
                break;
            }

            case ButtonPress:
                if (event.xbutton.button < 5)
                    window->mouse_buttons[event.xbutton.button] = true;
                break;

            case ButtonRelease:
                if (event.xbutton.button < 5)
                    window->mouse_buttons[event.xbutton.button] = false;
                break;

            case MotionNotify:
                window->mouse_x = event.xmotion.x;
                window->mouse_y = event.xmotion.y;
                break;
        }
    }
    return true;
}

bool is_key_down(window_t* window, uint16_t key_code) {
    return key_code < 65536 && window->keys[key_code];
}

bool is_mouse_button_down(window_t* window, unsigned int button) {
    return button < 5 && window->mouse_buttons[button];
}

void get_mouse_pos(window_t* window, int* x, int* y) {
    *x = std::clamp(window->mouse_x, 0, (int)window->width - 1);
    *y = std::clamp(window->mouse_y, 0, (int)window->height - 1);
}