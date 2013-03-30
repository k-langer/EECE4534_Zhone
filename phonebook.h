#ifndef PHONEBOOK_H
#define PHONEBOOK_H

/***
    Phonebook
        -keeps track of two remote Zhones' addresses
        -provides addresses when requested
***/

typedef struct {
    address_t address1;
    address_t address2;
} phonebook_t;


/***
    init
    
    pThis - pointer to phonebook object to initialize
    address1 - address to set as pThis's first entry
    address2 - address to set as pThis's second entry

    returns 0 on success or -1 on failure
***/
int phonebook_init(phonebook_t* pThis, address_t address1, address_t address2);

/***
    getAddress1/getAddress2

    pThis - pointer to phonebook from which to retrieve an address
    
    returns desired address as an address_t
***/
address_t phonebook_get_address1(phonebook_t* pThis);
address_t phonebook_get_address2(phonebook_t* pThis);

#endif
    
