INCLUDE(${TRILINOS_HOME_DIR}/cmake/ctest/drivers/pu241/mkl-12.0.4-options.cmake)
# hijack these to get modules paths into the include list; needed by CASLRAVE/ANC
SET(BLAS_INCLUDE_DIRS   ${MKLROOT}/include/gcc-4.6.1                     CACHE PATH     "Path to MKL BLAS Fortran modules compatible with GCC 4.6.1")
SET(LAPACK_INCLUDE_DIRS ${BLAS_INCLUDE_DIRS}                             CACHE PATH     "Path to MKL LAPACK Fortran modules compatible with GCC 4.6.1")

INCLUDE(${TRILINOS_HOME_DIR}/cmake/ctest/drivers/pu241/casl-vri-tpls.cmake)

SET(PVMLibraries_LIBRARY_DIRS /opt/gcc-4.5.1/tpls/pvm3/lib/LINUX64                    CACHE FILEPATH "")
SET(PVMHeaders_INCLUDE_DIRS /opt/gcc-4.5.1/tpls/pvm3/include                        CACHE FILEPATH "")
SET(HDF5_LIBRARY_NAMES "hdf5_hl;hdf5;hdf5_cpp"                               CACHE STRING   "")
SET(HDF5_LIBRARY_DIRS /opt/gcc-4.6.1/tpls/hdf5-1.8.5-patch1/lib              CACHE FILEPATH "")
SET(HDF5_INCLUDE_DIRS /opt/gcc-4.6.1/tpls/hdf5-1.8.5-patch1/include          CACHE FILEPATH "")
SET(EpetraExt_ENABLE_HDF5 OFF                                                CACHE BOOL "")

SET(TRILINOS_TOOLSET_BASE /opt/gcc-4.6.1/trilinos-toolset)
SET(Trilinos_EXTRA_LINK_FLAGS "-Wl,-rpath,${TRILINOS_TOOLSET_BASE}/lib64" CACHE STRING "")