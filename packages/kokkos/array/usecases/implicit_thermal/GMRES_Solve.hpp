/** \HEADER
 *************************************************************************
 *
 *                            Kokkos
 *                 Copyright 2010 Sandia Corporation
 *
 *  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
 *  the U.S. Government retains certain rights in this software.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *
 *  3. Neither the name of the Corporation nor the names of the
 *  contributors may be used to endorse or promote products derived from
 *  this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************
 */

#include <limits>
#include <WAXSBY.hpp>
#include <WAXPBY.hpp>
#include <Dot.hpp>
#include <CRSMatVec.hpp>
#include <impl/Kokkos_Timer.hpp>
template< typename Scalar , class DeviceType , unsigned N = 1 >
struct MultiVectorYSAX ;

template< typename Scalar >
struct MultiVectorYSAX<Scalar, KOKKOS_MACRO_DEVICE ,1> {
  typedef KOKKOS_MACRO_DEVICE     device_type ;
  typedef Scalar                  value_type ;
  typedef device_type::size_type  size_type ;

  Kokkos::MultiVectorView<Scalar,device_type> Y , X ;
  Kokkos::ValueView<Scalar,device_type>     A ;

  MultiVectorYSAX( const Kokkos::MultiVectorView<Scalar,device_type> & argY ,
                   const Kokkos::ValueView<Scalar,device_type>     & argA ,
                   const Kokkos::MultiVectorView<Scalar,device_type> & argX )
   : Y( argY ), X( argX ), A( argA ) {}

  KOKKOS_MACRO_DEVICE_FUNCTION
  void operator()( size_type iwork ) const
  { Y(iwork) -= X(iwork) * *A ; }
};


template <class Scalar , class DeviceType >
struct GMRES_Solve;


template<class Scalar>
struct GMRES_Solve<Scalar , KOKKOS_MACRO_DEVICE>
{

  typedef KOKKOS_MACRO_DEVICE    device_type;
  typedef device_type::size_type index_type ;
  typedef Kokkos::MultiVectorView<Scalar , device_type>  scalar_vector;
  typedef Kokkos::MultiVectorView<index_type , device_type>    index_vector;

  typedef Kokkos::ValueView<Scalar , device_type>     value;
  typedef Kokkos::ValueView<Scalar , Kokkos::DeviceHost> host_val;
  typedef Kokkos::MultiVectorYSAX<  Scalar , device_type , 1 > YSAX ;

  struct Norm2 {
    MultiVector R ;
    Value       inv ;
    size_type   j ;

    Norm2( const MultiVector & argR ,
              const Value       & argInv ,
              size_type argJ )
      : R( argR )
      , inv( argInv )
      , j( argJ )
      {}

    KOKKOS_MACRO_DEVICE_FUNCTION
    void operator()( Scalar & result ) const
    {
      const Scalar value = sqrt( result );
      R(j,j) = value ;
      if ( 0 < value ) {
        *inv = 1.0 / value ;
      }
      else {
        *inv = 0 ;
      }
    }
  };


  
  // Return megaflops / second for iterations

  static double run( scalar_vector & A_value ,
                     index_vector  & A_row ,
                     index_vector & A_offsets ,
                     scalar_vector & b ,
                     scalar_vector & x,
                     const size_t & num_iters)
  {
    const size_t rows = A_row.length()-1;


    // Solvers' working temporaries:
    scalar_vector r = 
      Kokkos::create_labeled_multivector<scalar_vector>("r",rows);
    scalar_vector Ax = 
      Kokkos::create_labeled_multivector<scalar_vector>("Ax",rows);

    //Compute initial residual r = b - A*x

    // create CRSMatVec object for A * x
    CRSMatVec<Scalar,device_type> A_mult_x(A_value, A_row , A_offsets , x , Ax);
    A_mult_x.apply(); // Ap = A * p

    // r = b - Ax
    Kokkos::parallel_for(
      rows , WAXSBY<Scalar , device_type>(one , b , one , Ax , r) );
    
    value beta  = Kokkos::create_value<Scalar , device_type>();
    Kokkos::deep_copy( zero, Scalar( 0 ) );

    // compute nrom2 of r
    Kokkos::parallel_reduce(rows, Norm2(r), beta);

    Scalar beta_copy;
    Kokkos::deep_copy(beta_copy,beta);
    if(beta_copy){
      //Stuff didn't work
      return 0.0;
    }

    //Allocate space for basis vectors Q. Q has as many rows as b and m+1 
    //columns.
    scalar_vector Q = 
      Kokkos::create_labeled_multivector<scalar_vector>("Q",rows, num_iters+1);

    //Allocate m+1 by m matrix H, and fill it with zeros.
    scalar_vector H = Kokkos::create_labeled_multivector<scalar_vector>(
      "r",
      num_iters, 
      num_iters+1);

    scalar_vector Qjtemp = create_labeled_multivector("Qjtemp", rows, 1);

    //Q(:, 0) = r ./ beta (elementwise division)
    value invbeta  = Kokkos::create_value<Scalar , device_type>();
    Kokkos::deep_copy( zero, Scalar( 1/beta_copy ) );
    Kokkos::parallel_for(rows, Scale(MultiVector(Q,0), invbeta))
    for(int j =1; i<= num_iters; ++j){
      //Q(:,j) = A * Q(:, j-1)
      CRSMatVec<Scalar,device_type> A_mult_q(
        A_value, A_row , A_offsets , MultiVector(Q, j-1) , MultiVector(Q, j));
      A_mult_q.apply();


      //H(0:j, j-1) = Q(:, 0:j-1)^* Q(:,j)
      NodeGEMM<Scalar, KOKKOS_MACRO_DEVICE>::GEMM(
        Teuchos::TRANS, Teuchos::NO_TRANS, 1.0, MultiVector(Q,0, j-1), 
        MultiVector(j), 0.0, MultiVector(H, j-1))


      // Q(:, j) = Q(:, j) - Q(:, 0:j-1) * H(0:j, j-1)
      for(int i =0; i<j; ++i){
        Kokkos::parallel_for(
          rows, YSAX(MultiVector(Q,j), H(i, j-1), MultiVector(Q,i)))
      }
     
      //H(j, j-1) = norm(Q(:,j),2) 
      
      //Q(:,j) = Q(:, j) / H(j, j-1)
      Kokkos::parallel_for( rows , Scale(MultiVector( Q , j ) , H(j,j-1)));
       
    }
};


