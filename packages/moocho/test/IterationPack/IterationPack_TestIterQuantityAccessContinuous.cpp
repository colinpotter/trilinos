// @HEADER
// ***********************************************************************
// 
// Moocho: Multi-functional Object-Oriented arCHitecture for Optimization
//                  Copyright (2003) Sandia Corporation
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
// Questions? Contact Roscoe A. Bartlett (rabartl@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#include <ostream>
#include <iomanip>
#include <vector>

#include "IterationPack_TestIterationPack.hpp"
#include "IterationPack_IterQuantityAccessContiguous.hpp"
#include "TestingHelperPack_update_success.hpp"

bool IterationPack::TestingPack::TestIterQuantityAccessContiguous(std::ostream* out)
{
  namespace rcp = MemMngPack;

  {
    // explicit instantiation test
    typedef std::vector<int> T;
    IterQuantityAccessContiguous<T> iq_v(
      1,"v"
#ifdef _MIPS_CXX
      ,Teuchos::RefCountPtr<Teuchos::AbstractFactoryStd<T,T> >(
        new Teuchos::AbstractFactoryStd<T,T>())
#endif			
      );
  }

  using std::endl;
  using std::setw;

  using TestingHelperPack::update_success;

  try {
  
//	int w = 15;
  int prec = 8;
  if(out) out->precision(prec);
  if(out) *out << std::boolalpha;
  bool success = true;
  bool result;

  int r;	// result

  if(out)
     *out	<< "\n********************************************\n"
        << "*** Testing IterQuantityAccessContiguous ***\n"
        << "********************************************\n";

  // Create a 1 storage and test it
  {

    if(out) 
      *out	<< "\n *** Test single storage ***\n"
          << "  IterQuantityAccessContiguous<int> x_cont(1,\"x\");\n"
          << "  IterQuantityAccess<int>& x = x_cont;\n";
    IterQuantityAccessContiguous<int> x_cont(
      1, "x"
#ifdef _MIPS_CXX
      ,Teuchos::RefCountPtr<Teuchos::AbstractFactoryStd<int,int> >(
        new Teuchos::AbstractFactoryStd<int,int>())
#endif			
      );
    IterQuantityAccess<int>& x = x_cont;

    if(out)
      *out<< "\n** Check state\n"
        << "x.has_storage_k(-300) == true : ";
    update_success( result = (x.has_storage_k(-300) == true), &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "x.has_storage_k(400) == true : ";
    update_success( result = x.has_storage_k(400) == true, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "!x.updated_k(-45) == true : ";
    update_success( result = x.updated_k(-45) == false, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "!x.updated_k(60) == true : ";
    update_success( result = x.updated_k(60) == false, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "\n** Perform an update and check state\n"
        << "x.set_k(0) = 5;\n\n";
    x.set_k(0) = 5;

    if(out)
      *out<< "x.get_k(0) == 5 : ";
    r = x.get_k(0);
    update_success( result = r == 5, &success );
    if(out) 
      *out<< r << " : " << result << endl;

    if(out)
      *out<< "!x.updated_k(-1) == true : ";
    update_success( result = x.updated_k(-1) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.updated_k(1) == true : ";
    update_success( result = x.updated_k(1) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.has_storage_k(-1) == true : ";
    update_success( result = x.has_storage_k(-1) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.has_storage_k(1) == true : ";
    update_success( result = x.has_storage_k(1) == true, &success );
    if(out)
      *out<< result << endl;

    if(out) 
      *out<< "\n** Do illegal set_k(), should throw NoStorageAvailable: x.set_k(-1) = 4 : ";
    try {
      x.set_k(-1) = 4;
      success = false;
      if(out)
        *out<< "false\n";
    }
    catch(const IterQuantity::NoStorageAvailable& excpt) {
      if(out)
        *out<< "As expected!  Caught IterQuantity::NoStorageAvailable: " << excpt.what() << " : true\n" << endl;
    }

    if(out)
      *out<< "\n** Do illegal get_k(), should throw QuanityNotSet: x.get_k(1) : ";
    try {
      x.get_k(1);
      success = false;
      if(out)
        *out<< "false\n";
    }
    catch(const IterQuantity::QuanityNotSet& excpt) {
      if(out) *out << "As expected!   Caught IterQuantity::QuanityNotSet: " << excpt.what() << " : true\n" << endl;
    }

    if(out) *out	<< "\nx.next_iteration();\n";
    x.next_iteration();

    if(out) *out<< "x.get_k(-1) == 5 : ";
    r = x.get_k(-1);
    update_success( result = r == 5, &success );
    if(out) 
      *out<< " : " << result << endl;

    if(out)
      *out<< "!x.updated_k(-2) == true : ";
    update_success( result = x.updated_k(-2) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.updated_k(0) == true : ";
    update_success( result = x.updated_k(0) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.has_storage_k(-2) == true : ";
    update_success( result = x.has_storage_k(-2) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.has_storage_k(0) == true : ";
    update_success( result = x.has_storage_k(0) == true, &success );
    if(out)
      *out<< result << endl;

    if(out) *out	<< "\nx.set_k(0,-1);\n\n";
    {
      x.set_k(0,-1);
    }
    
    if(out) *out	<< "x.get_k(0) == 5 : ";
    r = x.get_k(0);
    update_success( result = r  == 5, &success );
    if(out)
      *out<< r << " : " << result << endl;
  
    if(out)
      *out<< "x.will_loose_mem(0,1) == true : ";
    update_success( result = x.will_loose_mem(0,1) == true, &success );
    if(out)
      *out<< result << endl;

    if(out) *out	<< "\nx.set_k(1) = -4;\n\n";
    x.set_k(1) = -4;

    if(out)
      *out<< "x.get_k(1) == -4 : ";
    r = x.get_k(1);
    update_success( result = r == -4, &success );
    if(out)
      *out<< r << " : " << result << endl;

    if(out)
      *out<< "\nx.next_iteration();\n\n";
    x.next_iteration();

    if(out)
      *out<< "x.get_k(0) == -4 : ";
    r = x.get_k(0);
    update_success( result = r == -4, &success );
    if(out)
      *out<< r << " : " << result << endl;
    
  }

  // Create a 2 storage and test it
  {

    if(out)
      *out<< "\n*** Test dual storage ***\n"
        << "IterQuantityAccessContiguous<int> x_cont(2,\"x\");\n"
        << "IterQuantityAccess<int>& x = x_cont;\n";
    IterQuantityAccessContiguous<int> x_cont(
      2, "x"
#ifdef _MIPS_CXX
      ,Teuchos::RefCountPtr<Teuchos::AbstractFactoryStd<int,int> >(
        new Teuchos::AbstractFactoryStd<int,int>())
#endif			
      );
    IterQuantityAccess<int>& x = x_cont;

    if(out)
      *out<< "\n** Check state\n"
        << "x.has_storage_k(-300) == true : ";
    update_success( result = x.has_storage_k(-300) == true, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "x.has_storage_k(400) == true : ";
    update_success( result = x.has_storage_k(400) == true, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "!x.updated_k(-45) == true : ";
    update_success( result = x.updated_k(-45) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.updated_k(60) == true : ";
    update_success( result = x.updated_k(60) == false, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "\n** Perform an update and check state\n"
        << "x.set_k(0) = 5;\n\n";
    x.set_k(0) = 5;

    if(out)
      *out<< "x.get_k(0) == 5 : ";
    r = x.get_k(0);
    update_success( result = r == 5, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "!x.updated_k(-1) == true : ";
    update_success( result = x.updated_k(-1) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.updated_k(1) == true : ";
    update_success( result = x.updated_k(1) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.has_storage_k(-2) == true : ";
    update_success( result = x.has_storage_k(-2) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.has_storage_k(-1) == true : ";
    update_success( result = x.has_storage_k(-1) == true, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.has_storage_k(1) == true : ";
    update_success( result = x.has_storage_k(1) == true, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "\nx.set_k(-1) = 4;\n\n";
    x.set_k(-1) = 4;

    if(out)
      *out<< "x.get_k(-1) == 4 : ";
    r = x.get_k(-1);
    update_success( result = r == 4, &success );
    if(out)
      *out<< r << " : " << result << endl;

    if(out)
      *out<< "\n** Do illegal set_k(), should throw NoStorageAvailable: x.set_k(-2) = 4 : ";
    try {
      x.set_k(-2) = 4;
      success = false;
      if(out)
        *out<< "false\n";
    }
    catch(const IterQuantity::NoStorageAvailable& excpt) {
      if(out)
        *out<< "As expected!  Caught IterQuantity::NoStorageAvailable: " << excpt.what() << " : true\n" << endl;
    }

    if(out)
      *out<< "\n** Do illegal get_k(), should throw QuanityNotSet: x.get_k(1) : ";
    try {
      x.get_k(1);
      success = false;
      if(out)
        *out<< "false\n";
    }
    catch(const IterQuantity::QuanityNotSet& excpt) {
      if(out)
      *out<< "As expected!  Caught IterQuantity::QuanityNotSet: " << excpt.what() << " : true\n" << endl;
    }

    if(out)
      *out<< "\nx.next_iteration();\n\n";
    x.next_iteration();

    if(out)
      *out<< "x.get_k(-2) == 4 : ";
    r = x.get_k(-2);
    update_success( result = r == 4, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(-1) == 5 : ";
    r = x.get_k(-1);
    update_success( result = r == 5, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "!x.updated_k(-3) == true : ";
    update_success( result = x.updated_k(-3) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.updated_k(0) == true : ";
    update_success( result = x.updated_k(0) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.has_storage_k(-3) == true : ";
    update_success( result = x.has_storage_k(-3) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.has_storage_k(0) == true : ";
    update_success( result = x.has_storage_k(0) == true, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "x.set_k(0,-1);\n\n";
    {
      x.set_k(0,-1);
    }

    if(out)
      *out<< "x.get_k(0) == 5 : ";
    r = x.get_k(0);
    update_success( result = r == 5, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(-1) == 5 : ";
    r = x.get_k(-1);
    update_success( result = r == 5, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "!x.updated_k(-2) == true : ";
    update_success( result = x.updated_k(-2) == false, &success );
    if(out)
      *out<< result << endl;
  
    if(out)
      *out<< "!x.will_loose_mem(0,1) == true : ";
    update_success( result = x.will_loose_mem(0,1) == false, &success );
    if(out)
      *out<< result << endl;
      
    if(out)
      *out<< "x.will_loose_mem(-1,1) == true : ";
    update_success( result = x.will_loose_mem(-1,1) == true, &success );
    if(out)
      *out<< result << endl;
    
    if(out)
      *out<< "\nx.set_k(1) = -4;\n\n";
    x.set_k(1) = -4;
    
    if(out)
      *out<< "x.get_k(1) == -4 : ";
    r = x.get_k(1);
    update_success( result = r == -4, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(0) == 5 : ";
    r = x.get_k(0);
    update_success( result = r == 5, &success );
    if(out)
      *out<< r << " : " << result << endl;

    if(out)
      *out<< "\nx.next_iteration();\n\n";
    x.next_iteration();

    if(out)
      *out<< "x.get_k(0) == -4 : ";
    r = x.get_k(0);
    update_success( result = r == -4, &success );
    if(out)
      *out<< r << " : " << result << endl;
    
  }

  // Create a 4 storage and test it
  {
    if(out)
      *out<< "\n*** Test 4 storage ***\n"
        << "IterQuantityAccessContiguous<int> x_cont(4,\"x\");\n"
        << "IterQuantityAccess<int>& x = x_cont;\n";
    IterQuantityAccessContiguous<int> x_cont(
      4, "x"
#ifdef _MIPS_CXX
      ,Teuchos::RefCountPtr<Teuchos::AbstractFactoryStd<int,int> >(
        new Teuchos::AbstractFactoryStd<int,int>())
#endif			
      );
    IterQuantityAccess<int>& x = x_cont;

    if(out)
      *out<< "\n** Check state\n";
        
    if(out)
      *out<< "x.has_storage_k(-300) == true : ";
    update_success( result = x.has_storage_k(-300) == true, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.has_storage_k(400) == true : ";
    update_success( result = x.has_storage_k(400) == true, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.updated_k(-45) == true : ";
    update_success( result = x.updated_k(-45) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.updated_k(60) == true : ";
    update_success( result = x.updated_k(60) == false, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "\n** Perform an update and check state\n"
        << "x.set_k(0) = 1;\n\n";
    x.set_k(0) = 1;

    if(out)
      *out<< "x.get_k(0) == 1 : ";
    r = x.get_k(0);
    update_success( result = r == 1, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "!x.updated_k(-4) == true : ";
    update_success( result = x.updated_k(-4) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.updated_k(-3) == true : ";
    update_success( result = x.updated_k(-3) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.updated_k(-2) == true : ";
    update_success( result = x.updated_k(-2) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.updated_k(-1) == true : ";
    update_success( result = x.updated_k(-1) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.updated_k(0) == true : ";
    update_success( result = x.updated_k(0) == true, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.updated_k(1) == true : ";
    update_success( result = x.updated_k(1) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.has_storage_k(-4) == true : ";
    update_success( result = x.has_storage_k(-4) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.has_storage_k(-3) == true : ";
    update_success( result = x.has_storage_k(-3) == true, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.has_storage_k(-2) == true : ";
    update_success( result = x.has_storage_k(-2) == true, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.has_storage_k(-1) == true : ";
    update_success( result = x.has_storage_k(-1) == true, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.has_storage_k(0) == true : ";
    update_success( result = x.has_storage_k(0) == true, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.has_storage_k(1) == true : ";
    update_success( result = x.has_storage_k(1) == true, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "\nx.set_k(-1) = 2;\n\n";
    x.set_k(-1) = 2;
    
    if(out)
      *out<< "x.get_k(-1) == 2 : ";
    r = x.get_k(-1);
    update_success( result = r == 2, &success );
    if(out)
      *out<< r << " : " << result << endl;

    if(out)
      *out<< "\nx.set_k(-2) = 3;\n"
        << "x.set_k(-3) = 4;\n\n";
    x.set_k(-2) = 3;
    x.set_k(-3) = 4;
  
    if(out)
      *out<< "x.get_k(-2) == 3 : ";
    r = x.get_k(-2);
    update_success( result = r == 3, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(-3) == 4 : ";
    r = x.get_k(-3);
    update_success( result = r == 4, &success );
    if(out)
      *out<< r << " : " << result << endl;


    if(out)
      *out<< "\n** Do illegal set_k(), should throw NoStorageAvailable: x.set_k(-4) = 4 : ";
    try {
      x.set_k(-4) = 4;
      success = false;
      if(out)
        *out<< "false\n";
    }
    catch(const IterQuantity::NoStorageAvailable& excpt) {
      if(out)
        *out<< "As expected!  Caught IterQuantity::NoStorageAvailable: " << excpt.what() << " : true\n" << endl;
    }

    if(out)
      *out<< "\n** Do illegal get_k(), should throw QuanityNotSet: x.get_k(1) : ";
    try {
      x.get_k(1);
      success = false;
      if(out)
        *out<< "false\n";
    }
    catch(const IterQuantity::QuanityNotSet& excpt) {
      if(out)
        *out<< "As expected!  Caught IterQuantity::QuanityNotSet: " << excpt.what() << " : true" << endl;
    }

    if(out)
      *out<< "\nx.next_iteration();\n\n";
    x.next_iteration();

    if(out)
      *out<< "x.get_k(-4) == 4 : ";
    r = x.get_k(-4);
    update_success( result = r == 4, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(-3) == 3 : ";
    r = x.get_k(-3);
    update_success( result = r == 3, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(-2) == 2 : ";
    r = x.get_k(-2);
    update_success( result = r == 2, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(-1) == 1 : ";
    r = x.get_k(-1);
    update_success( result = r == 1, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "!x.updated_k(-5) == true : ";
    update_success( result = x.updated_k(-5) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.updated_k(0) == true : ";
    update_success( result = x.updated_k(0) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.has_storage_k(-5) == true : ";
    update_success( result = x.has_storage_k(-5) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.has_storage_k(0) == true : ";
    update_success( result = x.has_storage_k(0) == true, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "\nx.set_k(0,-1);\n\n";
    {
      x.set_k(0,-1);
    }

    if(out)
      *out<< "x.get_k(-3) == 3 : ";
    r = x.get_k(-3);
    update_success( result = r == 3, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(-2) == 2 : ";
    r = x.get_k(-2);
    update_success( result = r == 2, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(-1) == 1 : ";
    r = x.get_k(-1);
    update_success( result = r == 1, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(0) == 1 : ";
    r = x.get_k(0);
    update_success( result = r == 1, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "!x.updated_k(-4) == true : ";
    update_success( result = x.updated_k(-4) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "!x.will_loose_mem(-2,1) == true : ";
    update_success( result = x.will_loose_mem(-2,1) == false, &success );
    if(out)
      *out<< result << endl;
        
    if(out)
      *out<< "x.will_loose_mem(-3,1) == true : ";
    update_success( result = x.will_loose_mem(-3,1) == true, &success );
    if(out)
      *out<< result << endl;

    if(out)
      *out<< "\nx.set_k(1) = -4;\n\n";
    x.set_k(1) = -4;

    if(out)
      *out<< "x.get_k(-2) == 2 : ";
    r = x.get_k(-2);
    update_success( result = r == 2, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(-1) == 1 : ";
    r = x.get_k(-1);
    update_success( result = r == 1, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(0) == 1 : ";
    r = x.get_k(0);
    update_success( result = r == 1, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(1) == -4 : ";
    r = x.get_k(1);
    update_success( result = r == -4, &success );
    if(out)
      *out<< r << " : " << result << endl;

    if(out)
      *out<< "\nx.next_iteration();\n\n";
    x.next_iteration();

    if(out)
      *out<< "x.get_k(-2) == 1 : ";
    r = x.get_k(-2);
    update_success( result = r == 1, &success );
    if(out)
      *out<< r << " : " << result << endl;

    if(out)
      *out<< "x.get_k(-1) == 1 : ";
    r = x.get_k(-1);
    update_success( result = r == 1, &success );
    if(out)
      *out<< r << " : " << result << endl;
        
    if(out)
      *out<< "x.get_k(0) == -4 : ";
    r = x.get_k(0);
    update_success( result = r == -4, &success );
    if(out)
      *out<< r << " : " << result << endl;
  }

  if(success) {
    if(out)
      *out<< "\n*** Congradulations, all of the tests returned the expected results ***\n";
  }
  else {
    if(out)
      *out<< "\n*** Oops, at least one of the above tests did not return the expected results ***\n";
  }

  return success;

  } // end try
  catch(const std::exception& excpt) {
    if(out)
      *out<< "\nCaught a std::exception: " << excpt.what() << endl;
  }
  catch(...) {
    if(out)
      *out<< "\nCaught and unknown exception\n";
  }

  return false;
}
