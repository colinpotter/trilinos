// @HEADER
// ************************************************************************
//
//                           Intrepid Package
//                 Copyright (2007) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Pavel Bochev (pbboche@sandia.gov) or
//                    Denis Ridzal (dridzal@sandia.gov).
//
// ************************************************************************
// @HEADER

/** \file test_01.cpp
\brief  Unit test for the FunctionSpaceTools class, testing H-grad.
\author Created by D. Ridzal, P. Bochev, and K. Peterson.
*/

#include "Intrepid_FunctionSpaceTools.hpp"
#include "Intrepid_FieldContainer.hpp"
#include "Intrepid_CellTools.hpp"
#include "Intrepid_HGRAD_TET_C1_FEM.hpp"
#include "Intrepid_DefaultCubatureFactory.hpp"
#include "Intrepid_Utils.hpp"
#include "Teuchos_oblackholestream.hpp"
#include "Teuchos_RCP.hpp"

using namespace std;
using namespace Intrepid;

#define INTREPID_TEST_COMMAND( S )                                                                                  \
{                                                                                                                   \
  try {                                                                                                             \
    S ;                                                                                                             \
  }                                                                                                                 \
  catch (std::logic_error err) {                                                                                    \
      *outStream << "Expected Error ----------------------------------------------------------------\n";            \
      *outStream << err.what() << '\n';                                                                             \
      *outStream << "-------------------------------------------------------------------------------" << "\n\n";    \
  };                                                                                                                \
}


int main(int argc, char *argv[]) {

  // This little trick lets us print to std::cout only if
  // a (dummy) command-line argument is provided.
  int iprint     = argc - 1;
  Teuchos::RCP<std::ostream> outStream;
  Teuchos::oblackholestream bhs; // outputs nothing
  if (iprint > 0)
    outStream = Teuchos::rcp(&std::cout, false);
  else
    outStream = Teuchos::rcp(&bhs, false);

  // Save the format state of the original std::cout.
  Teuchos::oblackholestream oldFormatState;
  oldFormatState.copyfmt(std::cout);

  *outStream \
  << "===============================================================================\n" \
  << "|                                                                             |\n" \
  << "|                      Unit Test (FunctionSpaceTools)                         |\n" \
  << "|                                                                             |\n" \
  << "|     1) basic operator transformations and integration in HGRAD              |\n" \
  << "|                                                                             |\n" \
  << "|  Questions? Contact  Pavel Bochev (pbboche@sandia.gov) or                   |\n" \
  << "|                      Denis Ridzal (dridzal@sandia.gov).                     |\n" \
  << "|                                                                             |\n" \
  << "|  Intrepid's website: http://trilinos.sandia.gov/packages/intrepid           |\n" \
  << "|  Trilinos website:   http://trilinos.sandia.gov                             |\n" \
  << "|                                                                             |\n" \
  << "===============================================================================\n";


  int errorFlag = 0;
#ifdef HAVE_INTREPID_DEBUG
  int beginThrowNumber = TestForException_getThrowNumber();
  int endThrowNumber = beginThrowNumber + 5;
#endif

  typedef FunctionSpaceTools fst; 

  *outStream \
  << "\n"
  << "===============================================================================\n"\
  << "| TEST 1: exceptions                                                          |\n"\
  << "===============================================================================\n";

  try{
#ifdef HAVE_INTREPID_DEBUG
    *outStream << "\n >>>>> TESTING computeMeasure:\n";
    FieldContainer<double> a_2(2);
    FieldContainer<double> a_2_2(2, 2);
    FieldContainer<double> a_2_2_3(2, 2, 3);
    FieldContainer<double> a_2_2_3_3(2, 2, 3, 3);
    FieldContainer<double> a_2_2_2(2, 2, 2);
    FieldContainer<double> a_2_2_2_3(2, 2, 2, 3);
    FieldContainer<double> a_2_2_2_3_3(2, 2, 2, 3, 3);
    FieldContainer<double> a_2_2_2_2(2, 2, 2, 2);
    INTREPID_TEST_COMMAND( fst::computeMeasure<double>(a_2_2, a_2, a_2) );
    INTREPID_TEST_COMMAND( fst::computeMeasure<double>(a_2_2, a_2_2, a_2) );

    *outStream << "\n >>>>> TESTING integrate:\n";
    INTREPID_TEST_COMMAND( fst::integrate<double>(a_2_2_2_2, a_2_2_2, a_2_2_2, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::integrate<double>(a_2, a_2_2, a_2_2, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::integrate<double>(a_2, a_2_2_3, a_2_2_3, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::integrate<double>(a_2, a_2_2_3_3, a_2_2_3_3, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::integrate<double>(a_2_2, a_2_2, a_2_2_2, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::integrate<double>(a_2_2, a_2_2_3, a_2_2_2_3, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::integrate<double>(a_2_2, a_2_2_3_3, a_2_2_2_3_3, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::integrate<double>(a_2_2_2, a_2_2_2, a_2_2_2, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::integrate<double>(a_2_2_2, a_2_2_2_3, a_2_2_2_3, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::integrate<double>(a_2_2_2, a_2_2_2_3_3, a_2_2_2_3_3, COMP_CPP) );

    *outStream << "\n >>>>> TESTING operatorIntegral:\n";
    INTREPID_TEST_COMMAND( fst::operatorIntegral<double>(a_2_2_2, a_2_2, a_2_2_2, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::operatorIntegral<double>(a_2_2_2, a_2_2_2, a_2_2_2, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::operatorIntegral<double>(a_2_2_2, a_2_2_2_3, a_2_2_2_3, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::operatorIntegral<double>(a_2_2_2, a_2_2_2_3_3, a_2_2_2_3_3, COMP_CPP) );

    *outStream << "\n >>>>> TESTING functionalIntegral:\n";
    INTREPID_TEST_COMMAND( fst::functionalIntegral<double>(a_2_2, a_2_2_2_3_3, a_2_2_2, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::functionalIntegral<double>(a_2_2, a_2_2, a_2_2_2, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::functionalIntegral<double>(a_2_2, a_2_2_3, a_2_2_2_3, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::functionalIntegral<double>(a_2_2, a_2_2_3_3, a_2_2_2_3_3, COMP_CPP) );

    *outStream << "\n >>>>> TESTING dataIntegral:\n";
    INTREPID_TEST_COMMAND( fst::dataIntegral<double>(a_2, a_2, a_2_2_2, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::dataIntegral<double>(a_2, a_2_2, a_2_2, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::dataIntegral<double>(a_2, a_2_2_3, a_2_2_3, COMP_CPP) );
    INTREPID_TEST_COMMAND( fst::dataIntegral<double>(a_2, a_2_2_3_3, a_2_2_3_3, COMP_CPP) );
#endif
  }
  catch (std::logic_error err) {
    *outStream << "UNEXPECTED ERROR !!! ----------------------------------------------------------\n";
    *outStream << err.what() << '\n';
    *outStream << "-------------------------------------------------------------------------------" << "\n\n";
    errorFlag = -1000;
  };

#ifdef HAVE_INTREPID_DEBUG
  if (TestForException_getThrowNumber() != endThrowNumber)
    errorFlag++;
#endif

  *outStream \
  << "\n"
  << "===============================================================================\n"\
  << "| TEST 2: correctness of math operations                                      |\n"\
  << "===============================================================================\n";

  outStream->precision(20);

  try {
      shards::CellTopology cellType = shards::getCellTopologyData< shards::Tetrahedron<> >();   // cell type: tet

      /* Related to cubature. */
      DefaultCubatureFactory<double> cubFactory;                                                // create cubature factory
      int cubDegree = 2;                                                                        // cubature degree
      Teuchos::RCP<Cubature<double> > myCub = cubFactory.create(cellType, cubDegree);           // create default cubature
      int spaceDim = myCub->getDimension();                                                     // get spatial dimension 
      int numCubPoints = myCub->getNumPoints();                                                 // get number of cubature points

      /* Related to basis. */
      Basis_HGRAD_TET_C1_FEM<double, FieldContainer<double> > tetBasis;                         // create tet basis
      int numFields = tetBasis.getCardinality();                                                // get basis cardinality
 
      /* Cell geometries. */
      int numCells    = 4;
      int numNodes    = 4;
      int numCellData = numCells*numNodes*spaceDim;
      double tetnodes[] = {
        // tet 0
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
        // tet 1
        4.0, 5.0, 1.0,
       -6.0, 2.0, 0.0,
        4.0, -3.0, -1.0,
        0.0, 2.0, 5.0,
        // tet 2
        -6.0, -3.0, 1.0,
        9.0, 2.0, 1.0,
        8.9, 2.1, 0.9,
        8.9, 2.1, 1.1,
        // tet 3
        -6.0, -3.0, 1.0,
        12.0, 3.0, 1.0,
        2.9, 0.1, 0.9,
        2.9, 0.1, 1.1
      };

      /* Computational arrays. */
      FieldContainer<double> cub_points(numCubPoints, spaceDim);
      FieldContainer<double> cub_weights(numCubPoints);
      FieldContainer<double> cell_nodes(numCells, numNodes, spaceDim);
      FieldContainer<double> jacobian(numCells, numCubPoints, spaceDim, spaceDim);
      FieldContainer<double> jacobian_inv(numCells, numCubPoints, spaceDim, spaceDim);
      FieldContainer<double> jacobian_det(numCells, numCubPoints);
      FieldContainer<double> weighted_measure(numCells, numCubPoints);

      FieldContainer<double> grad_of_basis_at_cub_points(numFields, numCubPoints, spaceDim);
      FieldContainer<double> transformed_grad_of_basis_at_cub_points(numCells, numFields, numCubPoints, spaceDim);
      FieldContainer<double> weighted_transformed_grad_of_basis_at_cub_points(numCells, numFields, numCubPoints, spaceDim);
      FieldContainer<double> stiffness_matrices(numCells, numFields, numFields);

      FieldContainer<double> value_of_basis_at_cub_points(numFields, numCubPoints);
      FieldContainer<double> transformed_value_of_basis_at_cub_points(numCells, numFields, numCubPoints);
      FieldContainer<double> weighted_transformed_value_of_basis_at_cub_points(numCells, numFields, numCubPoints);
      FieldContainer<double> mass_matrices(numCells, numFields, numFields);

      /******************* START COMPUTATION ***********************/

      // get cubature points and weights
      myCub->getCubature(cub_points, cub_weights);

      // fill cell vertex array
      cell_nodes.setValues(tetnodes, numCellData);

      // compute geometric cell information
      CellTools<double>::setJacobian(jacobian, cub_points, cell_nodes, cellType);
      CellTools<double>::setJacobianInv(jacobian_inv, jacobian);
      CellTools<double>::setJacobianDet(jacobian_det, jacobian);

      // compute weighted measure
      fst::computeMeasure<double>(weighted_measure, jacobian_det, cub_weights);

      // Computing stiffness matrices:
      // tabulate gradients of basis functions at (reference) cubature points
      tetBasis.getValues(grad_of_basis_at_cub_points, cub_points, OPERATOR_GRAD);

      // transform gradients of basis functions into physical space
      fst::HGRADtransformGRAD<double>(transformed_grad_of_basis_at_cub_points,
                                      jacobian_inv,
                                      grad_of_basis_at_cub_points);

      // multiply with weighted measure
      fst::multiplyMeasure<double>(weighted_transformed_grad_of_basis_at_cub_points,
                                   weighted_measure,
                                   transformed_grad_of_basis_at_cub_points);

      // compute stiffness matrices
      fst::integrate<double>(stiffness_matrices,
                             transformed_grad_of_basis_at_cub_points,
                             weighted_transformed_grad_of_basis_at_cub_points,
                             COMP_CPP);


      // Computing mass matrices:
      // tabulate values of basis functions at (reference) cubature points
      tetBasis.getValues(value_of_basis_at_cub_points, cub_points, OPERATOR_VALUE);

      // transform gradients of basis functions into physical space
      fst::HGRADtransformVALUE<double>(transformed_value_of_basis_at_cub_points,
                                       value_of_basis_at_cub_points);

      // multiply with weighted measure
      fst::multiplyMeasure<double>(weighted_transformed_value_of_basis_at_cub_points,
                                   weighted_measure,
                                   transformed_value_of_basis_at_cub_points);

      // compute mass matrices
      fst::integrate<double>(mass_matrices,
                             transformed_value_of_basis_at_cub_points,
                             weighted_transformed_value_of_basis_at_cub_points,
                             COMP_CPP);

      /*******************  STOP COMPUTATION ***********************/


      /******************* START COMPARISON ***********************/
      string basedir = "./testdata";
      for (int cell_id = 0; cell_id < numCells; cell_id++) {

        stringstream namestream;
        string filename;
        namestream <<  basedir << "/mass_TET_FEM_P1" << "_" << "0" << cell_id+1 << ".dat";
        namestream >> filename;

        ifstream massfile(&filename[0]);
        if (massfile.is_open()) {
          if (compareToAnalytic<double>(&mass_matrices(cell_id, 0, 0), massfile, 1e-10, 0) > 0)
            errorFlag++;
          massfile.close();
        }
        else {
          errorFlag = -1;
          std::cout << "End Result: TEST FAILED\n";
          return errorFlag;
        }

        namestream.clear();
        namestream << basedir << "/stiff_TET_FEM_P1" << "_" << "0" << cell_id+1 << ".dat";
        namestream >> filename;

        ifstream stifffile(&filename[0]);
        if (stifffile.is_open())
        {
          if (compareToAnalytic<double>(&stiffness_matrices(cell_id, 0, 0), stifffile, 1e-10, 0) > 0)
            errorFlag++;
          stifffile.close();
        }
        else {
          errorFlag = -1;
          std::cout << "End Result: TEST FAILED\n";
          return errorFlag;
        }

      }

      /******************* STOP COMPARISON ***********************/

      *outStream << "\n";
  }
  catch (std::logic_error err) {
    *outStream << "UNEXPECTED ERROR !!! ----------------------------------------------------------\n";
    *outStream << err.what() << '\n';
    *outStream << "-------------------------------------------------------------------------------" << "\n\n";
    errorFlag = -1000;
  };


  if (errorFlag != 0)
    std::cout << "End Result: TEST FAILED\n";
  else
    std::cout << "End Result: TEST PASSED\n";

  // reset format state of std::cout
  std::cout.copyfmt(oldFormatState);

  return errorFlag;
}
