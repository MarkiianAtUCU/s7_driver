enum commands { set_out  = 0,
    set_in   = 1,
    set_low  = 2,
    set_high = 3,
    na       = 5};
extern int operate_GPIO(int gpio, enum commands cmd);

enum direction {in, out};
enum state {low, high};
