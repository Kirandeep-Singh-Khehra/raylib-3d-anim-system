#define sign(x) ((x < 0)?-1 : 1)
#define max(a, b) ((a > b)? a : b)
#define min(a, b) ((a < b)? a : b)
#define abs_(x) ((x < 0) ? -x : x)
#define clamp(x, a, b) ((x < a)? a : (x > b)? b : x)

#define rmod(x, y) (((x) % (y)) + (y)) % (y)

#define defer(fun) __attribute__((cleanup(fun)))

#define new_(x) (x*)malloc(sizeof(x));
#define new_n(n, x) malloc(n * sizeof(x));

