#include "task.h"
#include "os.h"
#include "printf.h"

static int svc_test(int n) {
    printf("Testing svc: %d\n", n);
    return n + 1;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
const void* svc_table[] = {
    svc_test
};
#pragma GCC diagnostic pop

