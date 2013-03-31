#include "commonTypes.h"
#include "ui.h"
#include "phonebook.h"
//#include "transfer_path.h"
//#include "receive_path.h"

#include <stdio.h>


int main() {
    phone_status_t testStatus = IDLE;
    phonebook_t phonebook;

    zhone_address_t a1 = 0xF0;
    zhone_address_t a2 = 0x11;

    phonebook_init( &phonebook, a1, a2 );
     
    if (testStatus == IDLE) {
        testStatus = CALL1;
    }
    if (testStatus == CALL1) {
        printf("calling zhone with #%u", phonebook.address1);
    }

    return 0;
}
