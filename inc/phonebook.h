#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include "commonTypes.h"

/***
    Phonebook
        -keeps track of two remote Zhones' addresses
        -provides addresses when requested
***/

typedef struct {
    zhone_address_t address1;
    zhone_address_t address2;
} phonebook_t;


/***
    init
    
    pThis - pointer to phonebook object to initialize
    address1 - address to set as pThis's first entry
    address2 - address to set as pThis's second entry

    returns PASS on success or FAIL on failure
***/
return_value_t phonebook_init(phonebook_t* pThis, zhone_address_t address1, zhone_address_t address2);

#endif
    
