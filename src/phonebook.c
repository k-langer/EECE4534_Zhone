#include "phonebook.h"
#include "commonTypes.h"

return_value_t phonebook_init(phonebook_t* pThis, zhone_address_t address1, zhone_address_t address2) {
    pThis->address1 = address1;
    pThis->address2 = address2;

	return PASS; 
}
