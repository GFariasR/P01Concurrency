# Process this file with autoconf to produce a configure script.
AC_INIT([LRUCache], [0.1], [gfariasr@users.noreply.github.com])

# Move as many of the generated files as possible to a subdirectory
AC_CONFIG_AUX_DIR(config)

# this is a C++ project.
AC_LANG([C++])

# We are going to use Automake to generate the makefiles, so initialize it
AM_INIT_AUTOMAKE([subdir-objects foreign add-mising])

# make Automake silent
AM_SILENT_RULES([yes])

# Create a config header, but with a distinctive name
AM_CONFIG_HEADER([LRUCache/LRUCache-config.hpp])

# There is only one Makefile in this project, it is non-recursive.
AC_CONFIG_FILES([Makefile])

# Make sure there is a C++ compiler, it supports C++11 and enable all warnings.
AC_PROG_CXX
# ... make sure we know how to create static libraries ...
AC_PROG_RANLIB

# ... check if there is a clang-format installed, allow the user to
# override the default by setting the CLANG_FORMAT environment
# variable ...
AC_CHECK_PROGS(CLANG_FORMAT, $CLANG_FORMAT clang-format, "")
AC_MSG_CHECKING([checking for clang-format version >= 3.8.0])
AS_IF([test "x$CLANG_FORMAT" != "x"], [
  ac_clang_format_version=`$CLANG_FORMAT --version | $AWK '{print $3}'`
  AX_COMPARE_VERSION([$ac_clang_format_version], [ge], [3.8.0],
    [AM_CONDITIONAL([FOUND_CLANG_FORMAT], [true])
     AC_MSG_RESULT([yes])],
    [AM_CONDITIONAL([FOUND_CLANG_FORMAT], [false])
     AC_MSG_RESULT([no])]
  )
], [
  AM_CONDITIONAL([FOUND_CLANG_FORMAT], [false])
  AC_MSG_RESULT([no])
])

# ... the code is going to be C++11, make sure the compiler supports it ...
AX_CXX_COMPILE_STDCXX_14(noext, mandatory)

# ... enable all warnings ...
AX_CXXFLAGS_WARN_ALL

# ... avoid spurious warnings from clang ...
AX_CHECK_COMPILE_FLAG([-Qunused-arguments],
  [AX_APPEND_FLAG(-Qunused-arguments, CXXFLAGS)], [])

# ... we need Boost with a version higher than 1.55 because Boost.Log ...
AX_BOOST_BASE([1.55], [], [
    AC_MSG_ERROR([
        unable to find a suitable Boost library, need version >= 1.55.
    ])
])

# ... then check for each specific Boost.* library we use ...
AX_BOOST_UNIT_TEST_FRAMEWORK

AC_OUTPUT
