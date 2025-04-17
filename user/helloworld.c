#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    const char *msg = "Hello, World xv6!\n";
    int len = strlen(msg);
    write(1, msg, len);
    exit(0);
}
