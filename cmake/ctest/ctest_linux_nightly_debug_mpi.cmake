#------------------------------------------------
# Nightly LINUX tests
# Debug-MPI
#------------------------------------------------
SET (CTEST_SOURCE_NAME Trilinos-CMake)
SET (TEST_TYPE nightly)
SET (BUILD_TYPE debug)

SET (CTEST_DASHBOARD_ROOT /var/dashboards)
SET (CTEST_CMAKE_COMMAND "\"${CMAKE_EXECUTABLE_NAME}\"")

# Options for Nightly builds
SET (CTEST_BACKUP_AND_RESTORE TRUE)
SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)
SET (CTEST_CVS_CHECKOUT
  "cvs -Q -d :ext:$ENV{USER}@software.sandia.gov:/space/CVS co ${CTEST_SOURCE_NAME}"
)
SET (CTEST_CVS_COMMAND
  "cvs -Q -d :ext:$ENV{USER}@software.sandia.gov:/space/CVS co ${CTEST_SOURCE_NAME}"
)

SET (CTEST_BINARY_NAME ${CTEST_SOURCE_NAME}-Build)
SET (CTEST_SOURCE_DIRECTORY "${CTEST_DASHBOARD_ROOT}/${CTEST_SOURCE_NAME}")
SET (CTEST_BINARY_DIRECTORY "${CTEST_DASHBOARD_ROOT}/${CTEST_BINARY_NAME}")

SET (CTEST_COMMAND 
  "\"${CTEST_EXECUTABLE_NAME}\" -D NightlyStart"
  "\"${CTEST_EXECUTABLE_NAME}\" -D NightlyConfigure"
  "\"${CTEST_EXECUTABLE_NAME}\" -D NightlyBuild"
  "\"${CTEST_EXECUTABLE_NAME}\" -D NightlySubmit"
  "\"${CTEST_EXECUTABLE_NAME}\" -D NightlyTest -R MPI"
  "\"${CTEST_EXECUTABLE_NAME}\" -D NightlySubmit -A \"${CTEST_BINARY_DIRECTORY}/CMakeCache.txt\" "
)

SET (CTEST_INITIAL_CACHE "

TRILINOS_ENABLE_ANASAZI:BOOL=ON
TRILINOS_ENABLE_EPETRA:BOOL=ON
TRILINOS_ENABLE_RBGEN:BOOL=ON
TRILINOS_ENABLE_TEUCHOS:BOOL=ON

TRILINOS_ENABLE_MPI:BOOL=ON
MPI_EXTRA_LIBRARY:STRING=" "

EPETRA_ENABLE_FORTRAN:BOOL=ON
RBGEN_ENABLE_EPETRA:BOOL=ON
TEUCHOS_ENABLE_COMPLEX:BOOL=ON
TEUCHOS_ENABLE_EXTENDED:BOOL=ON

TEUCHOS_ENABLE_TESTS:BOOL=ON
ANASAZI_ENABLE_TESTS:BOOL=ON
EPETRA_ENABLE_TESTS:BOOL=ON

TEUCHOS_ENABLE_EXAMPLES:BOOL=OFF
EPETRA_ENABLE_EXAMPLES:BOOL=OFF
ANASAZI_ENABLE_EXAMPLES:BOOL=OFF

BUILDNAME:STRING=$ENV{HOSTTYPE}-${TEST_TYPE}-${BUILD_TYPE}-mpi

CMAKE_BUILD_TYPE:STRING=${BUILD_TYPE}

CMAKE_EXE_LINKER_FLAGS:STRING=-lpthread

MPI_EXTRA_LIBRARY:STRING=""

MAKECOMMAND:STRING=gmake -j 4
")


