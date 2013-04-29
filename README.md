EECE4534_Zhone
==============

Four person Zigbee phone project for EECE4534. Spring 2013 semester class project. 
We want to make a low-power system. At a high level, Zhone consists of a User Interface,
a Transfer Path, a Receive Path, a Wireless Communicator, and a Phonebook. User 1 uses the 
UI to select a Zhone to call. User 2, the user of Zhone being called, is notified of an incoming
call and has the option of accepting or rejecting the call. Once User 2 has accepted the 
call, audio is encoded on one Zhone, sent to the other Zhone, decoded, and played back in both 
directions. Either Zhone can end the call at any time. The other Zhone is immediately notified a
nd both Zhones return to the main menu from which they can choose a Zhone to call or wait to receive
a call. Zhone is an improved alternative over walkie talkies because it allows for addressable devices,
thus preventing other devices from eavesdropping.    

Intstall and run: 
cd src; make; ./the_zhone
