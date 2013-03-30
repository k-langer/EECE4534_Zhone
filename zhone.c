#include "commonTypes.h"
#include "ui.h"
#include "phonebook.h"
#include <stdio.h>

int main() {
    status_t testStatus = IN_CALL;
    
    if (testStatus == IN_CALL) {
        testStatus = IDLE;
    }
    if (testStatus == IDLE) {
        printf("it works?");
    }

    return 0;
}
