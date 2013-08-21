# Set debug level Flags based on DEBUG env variable
# No compiler debug, no epopeia talking debug (default)
CCDEBUG	= -DDISABLE_DEBUG

ifeq ($(DEBUG),1)
  # Compiler debug, no epopeia talking debug
  CCDEBUG    	= -g -DDISABLE_DEBUG
endif  
ifeq ($(DEBUG),2)
  # Compiler debug AND epopeia talking debug
  CCDEBUG    	= -g
endif
