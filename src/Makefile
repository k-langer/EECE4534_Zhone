#############################################################################
# Makefile: THE ZHONE	  					            #
#############################################################################
#
# Aug 11, 2011 <agarwal>  Add description.
#	

# set CC to cross compiler 
CC = bfin-elf-gcc


# -- Compile Flags
CFLAGS = -Wall -specs=$(TLL6527M_C_DIR)/common/inc/spec/tll6527m 
# add debug flag 
CFLAGS += -g -O0

# -- Speex
SPEEX=../speex/libspeex/*.o
INC_SPEEX=-I ../speex/include


# -- Include Path
INC_PATH = -I ../inc

# -- Objects 
OBJS =  main.o \
		audioRx.o \
		audioTx.o \
        encoder.o \
        decoder.o \
		xbee.o \
		wire.o \
		uartTx.o \
		uartRx.o \
		wc.o \
		ui.o \
		display.o \
		receive_path.o \
		transfer_path.o \
		phonebook.o \
		zhone.o
		
# --- Libraries 	
LIBS     = -ltll6527mC   

# --- name of final binary 
TARGET = the_zhone

# Linker file 
LDFLAGS = -T$(TLL6527M_C_DIR)/common/inc/spec/tll6527m_sdram.lds -lm

# check that TLL6527M_C_DIR variable is defined before starting
ifeq ($(TLL6527M_C_DIR),)
$(error ERROR: TLL6527M_C_DIR environment var not set! Source environment settings first (e.g. source setup.csh))
endif

# --- Compilation 

# default rule
all: $(TARGET)

# link final binary
$(TARGET):$(OBJS) 
	$(CC) $(INC_PATH) $(INC_SPEEX) $(SPEEX) $(LIB_PATH) $(CFLAGS) -o $@ $^ $(LIBS) $(LDFLAGS)

# pattern rule for compiling into object files
.c.o:
	$(CC) $(INC_SPEEX) $(INC_PATH) -g -c $(CFLAGS)  -o $@ $<  

# --- Clean	
clean: 
	rm -rf $(TARGET) $(OBJS)		




