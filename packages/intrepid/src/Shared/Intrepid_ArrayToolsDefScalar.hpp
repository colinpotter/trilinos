// @HEADER
// ************************************************************************
//
//                           Intrepid Package
//                 Copyright (2007) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
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
// Questions? Contact Pavel Bochev  (pbboche@sandia.gov)
//                    Denis Ridzal  (dridzal@sandia.gov), or
//                    Kara Peterson (kjpeter@sandia.gov)
//
// ************************************************************************
// @HEADER

/** \file   Intrepid_ArrayToolsDefScalar.hpp
    \brief  Definition file for scalar multiply operations of the array tools interface.
    \author Created by P. Bochev and D. Ridzal.
*/

namespace Intrepid {

template<class Scalar, class ArrayOutFields, class ArrayInData, class ArrayInFields>
void ArrayTools::scalarMultiplyDataField(ArrayOutFields &     outputFields,
                                         const ArrayInData &  inputData,
                                         ArrayInFields &      inputFields,
                                         const bool           reciprocal) {

#ifdef HAVE_INTREPID_DEBUG
  TEUCHOS_TEST_FOR_EXCEPTION( (inputData.rank() != 2), std::invalid_argument,
                      ">>> ERROR (ArrayTools::scalarMultiplyDataField): Input data container must have rank 2.");
  if (outputFields.rank() <= inputFields.rank()) {
    TEUCHOS_TEST_FOR_EXCEPTION( ( (inputFields.rank() < 3) || (inputFields.rank() > 5) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataField): Input fields container must have rank 3, 4, or 5.");
    TEUCHOS_TEST_FOR_EXCEPTION( (outputFields.rank() != inputFields.rank()), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataField): Input and output fields containers must have the same rank.");
    TEUCHOS_TEST_FOR_EXCEPTION( (inputFields.dimension(0) != inputData.dimension(0) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataField): Zeroth dimensions (number of integration domains) of the fields and data input containers must agree!");
    TEUCHOS_TEST_FOR_EXCEPTION( ( (inputFields.dimension(2) != inputData.dimension(1)) && (inputData.dimension(1) != 1) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataField): Second dimension of the fields input container and first dimension of data input container (number of integration points) must agree or first data dimension must be 1!");
    for (int i=0; i<inputFields.rank(); i++) {
      std::string errmsg  = ">>> ERROR (ArrayTools::scalarMultiplyDataField): Dimension ";
      errmsg += (char)(48+i);
      errmsg += " of the input and output fields containers must agree!";
      TEUCHOS_TEST_FOR_EXCEPTION( (inputFields.dimension(i) != outputFields.dimension(i)), std::invalid_argument, errmsg );
    }
  }
  else {
    TEUCHOS_TEST_FOR_EXCEPTION( ( (inputFields.rank() < 2) || (inputFields.rank() > 4) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataField): Input fields container must have rank 2, 3, or 4.");
    TEUCHOS_TEST_FOR_EXCEPTION( (outputFields.rank() != inputFields.rank()+1), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataField): The rank of the input fields container must be one less than the rank of the output fields container.");
    TEUCHOS_TEST_FOR_EXCEPTION( ( (inputFields.dimension(1) != inputData.dimension(1)) && (inputData.dimension(1) != 1) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataField): First dimensions of fields input container and data input container (number of integration points) must agree or first data dimension must be 1!");
    TEUCHOS_TEST_FOR_EXCEPTION( ( inputData.dimension(0) != outputFields.dimension(0) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataField): Zeroth dimensions of fields output container and data input containers (number of integration domains) must agree!");
    for (int i=0; i<inputFields.rank(); i++) {
      std::string errmsg  = ">>> ERROR (ArrayTools::scalarMultiplyDataField): Dimensions ";
      errmsg += (char)(48+i);
      errmsg += " and ";
      errmsg += (char)(48+i+1);
      errmsg += " of the input and output fields containers must agree!";
      TEUCHOS_TEST_FOR_EXCEPTION( (inputFields.dimension(i) != outputFields.dimension(i+1)), std::invalid_argument, errmsg );
    }
  }
#endif

  // get sizes
  int invalRank      = inputFields.rank();
  int outvalRank     = outputFields.rank();
  int numCells       = outputFields.dimension(0);
  int numFields      = outputFields.dimension(1);
  int numPoints      = outputFields.dimension(2);
  int numDataPoints  = inputData.dimension(1);
  int dim1Tens       = 0;
  int dim2Tens       = 0;
  if (outvalRank > 3) {
    dim1Tens = outputFields.dimension(3);
    if (outvalRank > 4) {
      dim2Tens = outputFields.dimension(4);
    }
  }

  if (outvalRank == invalRank) {

    if (numDataPoints != 1) { // nonconstant data

      switch(invalRank) {
        case 3: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  outputFields(cl, bf, pt) = inputFields(cl, bf, pt)/inputData(cl, pt);
                } // P-loop
              } // F-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  outputFields(cl, bf, pt) = inputFields(cl, bf, pt)*inputData(cl, pt);
                } // P-loop
              } // F-loop
            } // C-loop
          }
        }// case 3
        break;

        case 4: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputFields(cl, bf, pt, iVec) = inputFields(cl, bf, pt, iVec)/inputData(cl, pt);
                  } // D1-loop
                } // P-loop
              } // F-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputFields(cl, bf, pt, iVec) = inputFields(cl, bf, pt, iVec)*inputData(cl, pt);
                  } // D1-loop
                } // P-loop
              } // F-loop
            } // C-loop
          }
        }// case 4
        break;

        case 5: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                    for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputFields(cl, bf, pt, iTens1, iTens2) = inputFields(cl, bf, pt, iTens1, iTens2)/inputData(cl, pt);
                    } // D2-loop
                  } // D1-loop
                } // F-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                    for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputFields(cl, bf, pt, iTens1, iTens2) = inputFields(cl, bf, pt, iTens1, iTens2)*inputData(cl, pt);
                    } // D2-loop
                  } // D1-loop
                } // F-loop
              } // P-loop
            } // C-loop
          }
        }// case 5
        break;

        default:
              TEUCHOS_TEST_FOR_EXCEPTION( !( (invalRank == 3) || (invalRank == 4) || (invalRank == 5) ), std::invalid_argument,
                                  ">>> ERROR (ArrayTools::scalarMultiplyDataField): This branch of the method is defined only for rank-3,4 or 5 containers.");
      }// invalRank

    }
    else { //constant data

      switch(invalRank) {
        case 3: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  outputFields(cl, bf, pt) = inputFields(cl, bf, pt)/inputData(cl, 0);
                } // P-loop
              } // F-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  outputFields(cl, bf, pt) = inputFields(cl, bf, pt)*inputData(cl, 0);
                } // P-loop
              } // F-loop
            } // C-loop
          }
        }// case 3
        break;

        case 4: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputFields(cl, bf, pt, iVec) = inputFields(cl, bf, pt, iVec)/inputData(cl, 0);
                  } // D1-loop
                } // P-loop
              } // F-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputFields(cl, bf, pt, iVec) = inputFields(cl, bf, pt, iVec)*inputData(cl, 0);
                  } // D1-loop
                } // P-loop
              } // F-loop
            } // C-loop
          }
        }// case 4
        break;

        case 5: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                    for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputFields(cl, bf, pt, iTens1, iTens2) = inputFields(cl, bf, pt, iTens1, iTens2)/inputData(cl, 0);
                    } // D2-loop
                  } // D1-loop
                } // F-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                    for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputFields(cl, bf, pt, iTens1, iTens2) = inputFields(cl, bf, pt, iTens1, iTens2)*inputData(cl, 0);
                    } // D2-loop
                  } // D1-loop
                } // F-loop
              } // P-loop
            } // C-loop
          }
        }// case 5
        break;

        default:
              TEUCHOS_TEST_FOR_EXCEPTION( !( (invalRank == 3) || (invalRank == 4) || (invalRank == 5) ), std::invalid_argument,
                                  ">>> ERROR (ArrayTools::scalarMultiplyDataField): This branch of the method is defined only for rank-3, 4 or 5 input containers.");

      } // invalRank
    } // numDataPoints

  }
  else {

    if (numDataPoints != 1) { // nonconstant data

      switch(invalRank) {
        case 2: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  outputFields(cl, bf, pt) = inputFields(bf, pt)/inputData(cl, pt);
                } // P-loop
              } // F-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  outputFields(cl, bf, pt) = inputFields(bf, pt)*inputData(cl, pt);
                } // P-loop
              } // F-loop
            } // C-loop
          }
        }// case 2
        break;

        case 3: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputFields(cl, bf, pt, iVec) = inputFields(bf, pt, iVec)/inputData(cl, pt);
                  } // D1-loop
                } // P-loop
              } // F-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputFields(cl, bf, pt, iVec) = inputFields(bf, pt, iVec)*inputData(cl, pt);
                  } // D1-loop
                } // P-loop
              } // F-loop
            } // C-loop
          }
        }// case 3
        break;

        case 4: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                    for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputFields(cl, bf, pt, iTens1, iTens2) = inputFields(bf, pt, iTens1, iTens2)/inputData(cl, pt);
                    } // D2-loop
                  } // D1-loop
                } // F-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                    for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputFields(cl, bf, pt, iTens1, iTens2) = inputFields(bf, pt, iTens1, iTens2)*inputData(cl, pt);
                    } // D2-loop
                  } // D1-loop
                } // F-loop
              } // P-loop
            } // C-loop
          }
        }// case 4
        break;

        default:
              TEUCHOS_TEST_FOR_EXCEPTION( !( (invalRank == 2) || (invalRank == 3) || (invalRank == 4) ), std::invalid_argument,
                                  ">>> ERROR (ArrayTools::scalarMultiplyDataField): This branch of the method is defined only for rank-2, 3 or 4 input containers.");
      }// invalRank

    }
    else { //constant data

      switch(invalRank) {
        case 2: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  outputFields(cl, bf, pt) = inputFields(bf, pt)/inputData(cl, 0);
                } // P-loop
              } // F-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  outputFields(cl, bf, pt) = inputFields(bf, pt)*inputData(cl, 0);
                } // P-loop
              } // F-loop
            } // C-loop
          }
        }// case 2
        break;

        case 3: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputFields(cl, bf, pt, iVec) = inputFields(bf, pt, iVec)/inputData(cl, 0);
                  } // D1-loop
                } // P-loop
              } // F-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputFields(cl, bf, pt, iVec) = inputFields(bf, pt, iVec)*inputData(cl, 0);
                  } // D1-loop
                } // P-loop
              } // F-loop
            } // C-loop
          }
        }// case 3
        break;

        case 4: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                    for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputFields(cl, bf, pt, iTens1, iTens2) = inputFields(bf, pt, iTens1, iTens2)/inputData(cl, 0);
                    } // D2-loop
                  } // D1-loop
                } // F-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int bf = 0; bf < numFields; bf++) {
                for(int pt = 0; pt < numPoints; pt++) {
                  for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                    for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputFields(cl, bf, pt, iTens1, iTens2) = inputFields(bf, pt, iTens1, iTens2)*inputData(cl, 0);
                    } // D2-loop
                  } // D1-loop
                } // F-loop
              } // P-loop
            } // C-loop
          }
        }// case 4
        break;

        default:
              TEUCHOS_TEST_FOR_EXCEPTION( !( (invalRank == 2) || (invalRank == 3) || (invalRank == 3) ), std::invalid_argument,
                                  ">>> ERROR (ArrayTools::scalarMultiplyDataField): This branch of the method is defined only for rank-2, 3 or 4 input containers.");

      } // invalRank
    } // numDataPoints

  } // end if (outvalRank = invalRank)

} // scalarMultiplyDataField



template<class Scalar, class ArrayOutData, class ArrayInDataLeft, class ArrayInDataRight>
void ArrayTools::scalarMultiplyDataData(ArrayOutData &           outputData,
                                        ArrayInDataLeft &        inputDataLeft,
                                        ArrayInDataRight &       inputDataRight,
                                        const bool               reciprocal) {

#ifdef HAVE_INTREPID_DEBUG
  TEUCHOS_TEST_FOR_EXCEPTION( (inputDataLeft.rank() != 2), std::invalid_argument,
                      ">>> ERROR (ArrayTools::scalarMultiplyDataData): Left input data container must have rank 2.");
  if (outputData.rank() <= inputDataRight.rank()) {
    TEUCHOS_TEST_FOR_EXCEPTION( ( (inputDataRight.rank() < 2) || (inputDataRight.rank() > 4) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataData): Right input data container must have rank 2, 3, or 4.");
    TEUCHOS_TEST_FOR_EXCEPTION( (outputData.rank() != inputDataRight.rank()), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataData): Right input and output data containers must have the same rank.");
    TEUCHOS_TEST_FOR_EXCEPTION( (inputDataRight.dimension(0) != inputDataLeft.dimension(0) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataData): Zeroth dimensions (number of integration domains) of the left and right data input containers must agree!");
    TEUCHOS_TEST_FOR_EXCEPTION( ( (inputDataRight.dimension(1) != inputDataLeft.dimension(1)) && (inputDataLeft.dimension(1) != 1) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataData): First dimensions of the left and right data input containers (number of integration points) must agree or first dimension of the left data input container must be 1!");
    for (int i=0; i<inputDataRight.rank(); i++) {
      std::string errmsg  = ">>> ERROR (ArrayTools::scalarMultiplyDataData): Dimension ";
      errmsg += (char)(48+i);
      errmsg += " of the right input and output data containers must agree!";
      TEUCHOS_TEST_FOR_EXCEPTION( (inputDataRight.dimension(i) != outputData.dimension(i)), std::invalid_argument, errmsg );
    }
  }
  else {
    TEUCHOS_TEST_FOR_EXCEPTION( ( (inputDataRight.rank() < 1) || (inputDataRight.rank() > 3) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataData): Right input data container must have rank 1, 2, or 3.");
    TEUCHOS_TEST_FOR_EXCEPTION( (outputData.rank() != inputDataRight.rank()+1), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataData): The rank of the right input data container must be one less than the rank of the output data container.");
    TEUCHOS_TEST_FOR_EXCEPTION( ( (inputDataRight.dimension(0) != inputDataLeft.dimension(1)) && (inputDataLeft.dimension(1) != 1) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataData): Zeroth dimension of the right input data container and first dimension of the left data input container (number of integration points) must agree or first dimension of the left data input container must be 1!");
    TEUCHOS_TEST_FOR_EXCEPTION( ( inputDataLeft.dimension(0) != outputData.dimension(0) ), std::invalid_argument,
                        ">>> ERROR (ArrayTools::scalarMultiplyDataData): Zeroth dimensions of data output and left data input containers (number of integration domains) must agree!");
    for (int i=0; i<inputDataRight.rank(); i++) {
      std::string errmsg  = ">>> ERROR (ArrayTools::scalarMultiplyDataData): Dimensions ";
      errmsg += (char)(48+i);
      errmsg += " and ";
      errmsg += (char)(48+i+1);
      errmsg += " of the right input and output data containers must agree!";
      TEUCHOS_TEST_FOR_EXCEPTION( (inputDataRight.dimension(i) != outputData.dimension(i+1)), std::invalid_argument, errmsg );
    }
  }
#endif

  // get sizes
  int invalRank      = inputDataRight.rank();
  int outvalRank     = outputData.rank();
  int numCells       = outputData.dimension(0);
  int numPoints      = outputData.dimension(1);
  int numDataPoints  = inputDataLeft.dimension(1);
  int dim1Tens       = 0;
  int dim2Tens       = 0;
  if (outvalRank > 2) {
    dim1Tens = outputData.dimension(2);
    if (outvalRank > 3) {
      dim2Tens = outputData.dimension(3);
    }
  }

  if (outvalRank == invalRank) {

    if (numDataPoints != 1) { // nonconstant data

      switch(invalRank) {
        case 2: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                  outputData(cl, pt) = inputDataRight(cl, pt)/inputDataLeft(cl, pt);
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                  outputData(cl, pt) = inputDataRight(cl, pt)*inputDataLeft(cl, pt);
              } // P-loop
            } // C-loop
          }
        }// case 2
        break;

        case 3: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputData(cl, pt, iVec) = inputDataRight(cl, pt, iVec)/inputDataLeft(cl, pt);
                } // D1-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputData(cl, pt, iVec) = inputDataRight(cl, pt, iVec)*inputDataLeft(cl, pt);
                } // D1-loop
              } // P-loop
            } // C-loop
          }
        }// case 3
        break;

        case 4: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                  for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputData(cl, pt, iTens1, iTens2) = inputDataRight(cl, pt, iTens1, iTens2)/inputDataLeft(cl, pt);
                  } // D2-loop
                } // D1-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                  for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputData(cl, pt, iTens1, iTens2) = inputDataRight(cl, pt, iTens1, iTens2)*inputDataLeft(cl, pt);
                  } // D2-loop
                } // D1-loop
              } // P-loop
            } // C-loop
          }
        }// case 4
        break;

        default:
              TEUCHOS_TEST_FOR_EXCEPTION( !( (invalRank == 2) || (invalRank == 3) || (invalRank == 4) ), std::invalid_argument,
                                  ">>> ERROR (ArrayTools::scalarMultiplyDataData): This branch of the method is defined only for rank-2, 3 or 4 containers.");
      }// invalRank

    }
    else { // constant left data

      switch(invalRank) {
        case 2: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                  outputData(cl, pt) = inputDataRight(cl, pt)/inputDataLeft(cl, 0);
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                  outputData(cl, pt) = inputDataRight(cl, pt)*inputDataLeft(cl, 0);
              } // P-loop
            } // C-loop
          }
        }// case 2
        break;

        case 3: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputData(cl, pt, iVec) = inputDataRight(cl, pt, iVec)/inputDataLeft(cl, 0);
                } // D1-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputData(cl, pt, iVec) = inputDataRight(cl, pt, iVec)*inputDataLeft(cl, 0);
                } // D1-loop
              } // P-loop
            } // C-loop
          }
        }// case 3
        break;

        case 4: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                  for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputData(cl, pt, iTens1, iTens2) = inputDataRight(cl, pt, iTens1, iTens2)/inputDataLeft(cl, 0);
                  } // D2-loop
                } // D1-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                  for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputData(cl, pt, iTens1, iTens2) = inputDataRight(cl, pt, iTens1, iTens2)*inputDataLeft(cl, 0);
                  } // D2-loop
                } // D1-loop
              } // P-loop
            } // C-loop
          }
        }// case 4
        break;

        default:
              TEUCHOS_TEST_FOR_EXCEPTION( !( (invalRank == 2) || (invalRank == 3) || (invalRank == 4) ), std::invalid_argument,
                                  ">>> ERROR (ArrayTools::scalarMultiplyDataData): This branch of the method is defined only for rank-2, 3 or 4 input containers.");

      } // invalRank
    } // numDataPoints

  }
  else {

    if (numDataPoints != 1) { // nonconstant data

      switch(invalRank) {
        case 1: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                  outputData(cl, pt) = inputDataRight(pt)/inputDataLeft(cl, pt);
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                  outputData(cl, pt) = inputDataRight(pt)*inputDataLeft(cl, pt);
              } // P-loop
            } // C-loop
          }
        }// case 1
        break;

        case 2: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputData(cl, pt, iVec) = inputDataRight(pt, iVec)/inputDataLeft(cl, pt);
                } // D1-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputData(cl, pt, iVec) = inputDataRight(pt, iVec)*inputDataLeft(cl, pt);
                } // D1-loop
              } // P-loop
            } // C-loop
          }
        }// case 2
        break;

        case 3: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                  for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputData(cl, pt, iTens1, iTens2) = inputDataRight(pt, iTens1, iTens2)/inputDataLeft(cl, pt);
                  } // D2-loop
                } // D1-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                  for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputData(cl, pt, iTens1, iTens2) = inputDataRight(pt, iTens1, iTens2)*inputDataLeft(cl, pt);
                  } // D2-loop
                } // D1-loop
              } // P-loop
            } // C-loop
          }
        }// case 3
        break;

        default:
              TEUCHOS_TEST_FOR_EXCEPTION( !( (invalRank == 1) || (invalRank == 2) || (invalRank == 3) ), std::invalid_argument,
                                  ">>> ERROR (ArrayTools::scalarMultiplyDataData): This branch of the method is defined only for rank-1, 2 or 3 input containers.");
      }// invalRank

    }
    else { //constant data

      switch(invalRank) {
        case 1: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                  outputData(cl, pt) = inputDataRight(pt)/inputDataLeft(cl, 0);
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                  outputData(cl, pt) = inputDataRight(pt)*inputDataLeft(cl, 0);
              } // P-loop
            } // C-loop
          }
        }// case 1
        break;

        case 2: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                  for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputData(cl, pt, iVec) = inputDataRight(pt, iVec)/inputDataLeft(cl, 0);
                } // D1-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iVec = 0; iVec < dim1Tens; iVec++) {
                    outputData(cl, pt, iVec) = inputDataRight(pt, iVec)*inputDataLeft(cl, 0);
                } // D1-loop
              } // P-loop
            } // C-loop
          }
        }// case 2
        break;

        case 3: {
          if (reciprocal) {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                  for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputData(cl, pt, iTens1, iTens2) = inputDataRight(pt, iTens1, iTens2)/inputDataLeft(cl, 0);
                  } // D2-loop
                } // D1-loop
              } // P-loop
            } // C-loop
          }
          else {
            for(int cl = 0; cl < numCells; cl++) {
              for(int pt = 0; pt < numPoints; pt++) {
                for( int iTens1 = 0; iTens1 < dim1Tens; iTens1++) {
                  for( int iTens2 = 0; iTens2 < dim2Tens; iTens2++) {
                      outputData(cl, pt, iTens1, iTens2) = inputDataRight(pt, iTens1, iTens2)*inputDataLeft(cl, 0);
                  } // D2-loop
                } // D1-loop
              } // P-loop
            } // C-loop
          }
        }// case 3
        break;

        default:
              TEUCHOS_TEST_FOR_EXCEPTION( !( (invalRank == 1) || (invalRank == 2) || (invalRank == 3) ), std::invalid_argument,
                                  ">>> ERROR (ArrayTools::scalarMultiplyDataData): This branch of the method is defined only for rank-1, 2 or 3 input containers.");

      } // invalRank
    } // numDataPoints

  } // end if (outvalRank = invalRank)

} // scalarMultiplyDataData

} // end namespace Intrepid
