/*
//@HEADER
// ************************************************************************
// 
//          Kokkos: Node API and Parallel Node Kernels
//              Copyright (2008) Sandia Corporation
// 
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ************************************************************************
//@HEADER
*/

#include <sstream>
#include <iostream>
#include <math.h>

#include <Kokkos_DeviceHost.hpp>
#include <Kokkos_DeviceHost_ValueView.hpp>
#include <Kokkos_DeviceHost_MultiVectorView.hpp>
#include <Kokkos_DeviceHost_MDArrayView.hpp>
#include <Kokkos_DeviceHost_ParallelFor.hpp>
#include <Kokkos_DeviceHost_ParallelReduce.hpp>

#include <Kokkos_DeviceTPI.hpp>
#include <Kokkos_DeviceTPI_ValueView.hpp>
#include <Kokkos_DeviceTPI_MultiVectorView.hpp>
#include <Kokkos_DeviceTPI_MDArrayView.hpp>
#include <Kokkos_DeviceTPI_ParallelFor.hpp>
#include <Kokkos_DeviceTPI_ParallelReduce.hpp>

#include <Kokkos_DeviceTPI_macros.hpp>
#include <Element.hpp>
#include <CRSMatrixGatherFill.hpp>
#include <Dirichlet.hpp>
#include <driver.hpp>
#include <Kokkos_DeviceClear_macros.hpp>

namespace Test{

  void 
  test_TPI (const int beg, 
	    const int end, 
	    const int runs, 
	    const int num_iters, 
	    const int threads)
  {
    Kokkos::DeviceTPI::initialize (threads);

    std::ostringstream label_double, label_float;
    label_double << "TPI[" << threads << "]-double";
    label_float  << "TPI[" << threads << "]-float";

    MiniImplTherm<double, Kokkos::DeviceTPI >::driver (label_double.str().c_str(), beg, end, runs, num_iters);
    MiniImplTherm<float , Kokkos::DeviceTPI >::driver (label_float.str().c_str() , beg, end, runs, num_iters);

    Kokkos::DeviceTPI::finalize();
  }

}// namespace

