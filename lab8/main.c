//
// Created by Koschei on 2024/3/1.
//

#include <stdlib.h>
#include "sem.h"

int main() {
    init();
    p("S0", 2);

    p("S1", 1);
    p("S1", 2);
    p("S1", 3);
    p("S1", 4);
    p("S1", 5);

    p("S2", 2);
    p("S2", 2);
    p("S2", 3);

    p("S3", 3);
    p("S3", 6);

    p("S4", 1);

    print_resources();

    v("S1");
    v("S2");
    v("S3");

    print_resources();
}
