INC                  = -I. -I$(top_builddir)/build/include/ -g
AM_CPPFLAGS          = $(INC)
AM_CXXFLAGS          = $(AM_CPPFLAGS)

LINK                 = -lctcxx -luser -loneD -lzeroD -lequil -lkinetics -ltransport -lthermo 
LINK                += -lctnumerics -lctmath -ltpx -lctspectra -lconverters -lctbase -lcvode 
LINK                += -lctlapack -lctblas -lctf2c -lm -lstdc++
AM_LDFLAGS           = -L$(top_builddir)/build/lib/ 
LIBS                 =  $(LINK)

check_PROGRAMS         = statmech_test \
                         statmech_test_poly \
                         statmech_test_Fe

library_includedir   = $(INC)

#-----------------------
# Cantera DH graph test
#-----------------------

statmech_test_SOURCES      = statmech_test.cpp
statmech_test_poly_SOURCES = statmech_test_poly.cpp
statmech_test_Fe_SOURCES   = statmech_test_Fe.cpp

TESTS_ENVIRONMENT = 	
TESTS             = statmech_test \
                    statmech_test_poly \
                    statmech_test_Fe

EXTRA_DIST = runtest_stat 