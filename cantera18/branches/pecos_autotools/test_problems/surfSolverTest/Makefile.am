cc_sources = surfaceSolver.cpp
cc2_sources = surfaceSolver2.cpp

INC                  = -I. -I$(top_builddir)/build/include/
AM_CPPFLAGS          = $(INC)
AM_CXXFLAGS          = $(AM_CPPFLAGS)

LINK                 = -luser -loneD -lzeroD -lequil -lkinetics -ltransport -lthermo 
LINK                += -lctnumerics -lctmath -ltpx -lctspectra -lconverters -lctbase -lcvode 
LINK                += -lctlapack -lctblas -lctf2c -lctcxx -lm  -lctf2c -lstdc++
AM_LDFLAGS           = -L$(top_builddir)/build/lib/ 
LIBS                 =  $(LINK)

bin_PROGRAMS         = surfaceSolver surfaceSolver2
library_includedir   = $(INC)

#-----------------------
# Cantera DH graph test
#-----------------------

surfaceSolver_SOURCES  = $(cc_sources)
surfaceSolver2_SOURCES = $(cc2_sources)

TESTS_ENVIRONMENT = 
TESTS             = runtest runtest2

EXTRA_DIST = runtest runtest2