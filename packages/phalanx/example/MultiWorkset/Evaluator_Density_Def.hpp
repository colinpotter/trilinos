// @HEADER
// ************************************************************************
// 
//        Phalanx: A Partial Differential Equation Field Evaluation 
//       Kernel for Flexible Management of Complex Dependency Chains
//                  Copyright (2008) Sandia Corporation
// 
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
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
// 
// Questions? Contact Roger Pawlowski (rppawlo@sandia.gov), Sandia
// National Laboratories.
// 
// ************************************************************************
// @HEADER

//**********************************************************************
PHX_EVALUATOR_CTOR_NAMESPACE(MyApp,Density,p) :
  density("Density", p.get< Teuchos::RCP<PHX::DataLayout> >("Data Layout") ),
  temp("Temperature", p.get< Teuchos::RCP<PHX::DataLayout> >("Data Layout") )
{ 
  this->addEvaluatedField(density);
  this->addDependentField(temp);
  this->setName("Density");
}

//**********************************************************************
PHX_POST_REGISTRATION_SETUP(MyApp::Density,fm)
{
  this->utils.setFieldData(density,fm);
  this->utils.setFieldData(temp,fm);

  // This line and the member workset_size are used for test converage
  // of the function getWorksetSize().  It is not needed for this
  // provider. Please ignore the following line!
  workset_size = this->utils.getWorksetSize(density,fm);
}

//**********************************************************************
PHX_EVALUATE_FIELDS(MyApp::Density,workset)
{ 
  for (int i = 0; i < density.size(); ++i)
    density[i] =  temp[i] * temp[i];
}

//**********************************************************************
