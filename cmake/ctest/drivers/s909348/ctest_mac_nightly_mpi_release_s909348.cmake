
INCLUDE("${CTEST_SCRIPT_DIRECTORY}/TrilinosCTestDriverCore.s909348.gcc.cmake")

#
# Set the options specific to this build case
#

SET(COMM_TYPE MPI)
SET(BUILD_TYPE RELEASE)
SET(BUILD_DIR_NAME MPI_RELEASE)

#SET(CTEST_DO_COVERAGE_TESTING TRUE)
#SET(CTEST_DO_MEMORY_TESTING TRUE)

SET(Trilinos_EXCLUDE_PACKAGES ${Trilinos_EXCLUDE_PACKAGES} Zoltan )

SET( EXTRA_CONFIGURE_OPTIONS
  "-DCMAKE_CXX_FLAGS:STRING=${COVERAGE_FLAGS}"
  "-DCMAKE_C_FLAGS:STRING=${COVERAGE_FLAGS}"
  "-DCMAKE_Fortran:STRING=${COVERAGE_FLAGS}"
  "-DCMAKE_EXE_LINKER_FLAGS:STRING=${COVERAGE_FLAGS}"
  "-DDART_TESTING_TIMEOUT:STRING=600"
  "-DCMAKE_VERBOSE_MAKEFILE:BOOL=TRUE"
  "-DTrilinos_ENABLE_EXPLICIT_INSTANTIATION:BOOL=ON"
  )

#
# Set the rest of the system-specific options and run the dashboard build/test
#

TRILINOS_SYSTEM_SPECIFIC_CTEST_DRIVER()
