# P01 uses a non-recursive make, this is the only Makefile.

instdir = $(prefix)/lib

inst_LIBRARIES = \

noinst_LIBRARIES = \

unit_tests = \
  LRUCache/ut_LRUCache

P01_lib_LRUCache_adir = $(includedir)
P01_lib_LRUCache_a_SOURCES = \
  P01/LRUCache/LRUCache.cpp
  P01/LRUCache/LRUCache.cpp

P01_lib_LRUCache_a_HEADERS = \
  P01/LRUCache/LRUCache.hpp

P01_lib_LRUCache_a_LIBADD =

benchmarks = \

examples = \

noinst_PROGRAMS = $(examples) $(benchmarks)

check_PROGRAMS = $(unit_tests)
TESTS = $(check_PROGRAMS)

AM_CXXFLAGS = \
  -DJB_SYSCONFDIR=\"$(sysconfdir)\" \
  -DJB_BINDIR=\"$(bindir)\" \
  $(BOOST_CPPFLAGS) \
  -DBOOST_LOG_DYN_LINK \

LDFLAGS = \
  $(BOOST_LDFLAGS)

LIBS = \

# Common configuration for all unit tests
UT_CPPFLAGS = \
  -DBOOST_TEST_DYN_LINK \
  -DBOOST_TEST_MAIN \
  -DSKYE_USE_BOOST_UNIT_TEST_FRAMEWORK \
  $(CPPFLAGS)

P01_ut_libs = \
  $(BOOST_UNIT_TEST_FRAMEWORK_LIB)

################################################################
# If CLANG_FORMAT was found, 
################################################################
if FOUND_CLANG_FORMAT
check-style:
	$(AM_V_at)find $(top_srcdir) -name '*.[hic]pp' | \
	  xargs $(CLANG_FORMAT) -i
	$(AM_V_at)git diff --exit-code && echo "clang-format checks passed"
else
check-style:
	$(AM_V_at)echo "clang-format not found, cannot check style" && exit 1
endif !FOUND_CLANG_FORMAT
.PHONY: check-style

################################################################
# unit tests
################################################################

P01_ut_LRUCache_SOURCES = LRUCache/ut_LRUCache.cpp
P01_ut_LRUCache_CPPFLAGS = $(UT_CPPFLAGS) -DBOOST_TEST_MODULE=P01_ut_LRUCache
P01_ut_LRUCache_LDADD = $(P01_ut_libs)
