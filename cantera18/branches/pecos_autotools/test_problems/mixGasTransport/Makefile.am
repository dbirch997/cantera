cc_sources = mixGasTransport.cpp

INC                  = -I. -I$(top_builddir)/build/include/ -g
AM_CPPFLAGS          = $(INC)
AM_CXXFLAGS          = $(AM_CPPFLAGS)

LINK                 = -lctcxx -luser -loneD -lzeroD -lequil -lkinetics -ltransport -lthermo 
LINK                += -lctnumerics -lctmath -ltpx -lctspectra -lconverters -lctbase -lcvode 
LINK                += -lctlapack -lctblas -lctf2c -lm -lstdc++
AM_LDFLAGS           = -L$(top_builddir)/build/lib/ 
LIBS                 =  $(LINK)

bin_PROGRAMS         = mixGasTransport
library_includedir   = $(INC)

#-----------------------
# Cantera DH graph test
#-----------------------

mixGasTransport_SOURCES = $(cc_sources)


TESTS_ENVIRONMENT = 
TESTS             = runtest

EXTRA_DIST = runtest