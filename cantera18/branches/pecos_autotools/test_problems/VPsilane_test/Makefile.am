cc_sources = silane_equil.cpp

INC                  = -I. -I$(top_builddir)/build/include/
AM_CPPFLAGS          = $(INC)
AM_CXXFLAGS          = $(AM_CPPFLAGS)

LINK                 = -luser -loneD -lzeroD -lequil -lkinetics -ltransport -lthermo 
LINK                += -lctnumerics -lctmath -ltpx -lctspectra -lconverters -lctbase -lcvode 
LINK                += -lctlapack -lctblas -lctf2c -lctcxx -lm  -lctf2c -lstdc++
AM_LDFLAGS           = -L$(top_builddir)/build/lib/ 
LIBS                 =  $(LINK)

bin_PROGRAMS         =VPsilane_test
library_includedir   = $(INC)

#-----------------------
# Cantera DH graph test
#-----------------------

VPsilane_test_SOURCES = $(cc_sources)

TESTS_ENVIRONMENT = 
TESTS             = runtest

EXTRA_DIST = runtest