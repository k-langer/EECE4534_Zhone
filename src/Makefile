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
CFLAGS += -g -lm -O0


# -- Include Path
INC_PATH = -I ../inc
INC_SPEEX = -I ../speex/include

# -- Objects 
OBJS =  zhone.o \
	receive_path.o \
	transfer_path.o \
	phonebook.o \
	encoder.o \
	decoder.o \
	audioTx.o \
	wc.o \
	ui.o \
	audioRx.o

# --- Libraries 	
LIBS     = -ltll6527mC   

# --- Speex
SPEEX    = ../speex/libspeex/*.o
  
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
	$(CC) $(INC_PATH) $(LIB_PATH) $(INC_SPEEX) $(SPEEX) $(CFLAGS) $(OBJS) -o $@ $^ $(LIBS) $(LDFLAGS)

# pattern rule for compiling into object files
.c.o:
	$(CC)  $(INC_PATH) $(INC_SPEEX) -g -c $(CFLAGS)  -o $@ $<  

# --- Clean	
clean: 
	rm -rf $(TARGET) $(OBJS)		




