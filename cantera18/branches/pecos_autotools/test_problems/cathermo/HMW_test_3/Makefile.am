cc_sources = HMW_test_3.cpp

INC                  = -I$(top_builddir)/build/include/
AM_CPPFLAGS          = $(INC)
AM_CXXFLAGS          = $(AM_CPPFLAGS)

LINK                 = -lctcxx -luser -loneD -lzeroD -lequil -lkinetics -ltransport -lthermo 
LINK                += -lctnumerics -lctmath -ltpx -lctspectra -lconverters -lctbase -lcvode 
LINK                += -lctlapack -lctblas -lctf2c -lm -lstdc++
AM_LDFLAGS           = -L$(top_builddir)/build/lib/ 
LIBS                 =  $(LINK)

bin_PROGRAMS         = HMW_test_3
library_includedir   = $(INC)

#-----------------------
# Cantera DH graph test
#-----------------------

HMW_test_3_SOURCES = $(cc_sources)

TESTS_ENVIRONMENT = 
TESTS             = runtest

EXTRA_DIST = runtest