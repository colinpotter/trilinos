// @HEADER
// ***********************************************************************
// 
//                    Teuchos: Common Tools Package
//                 Copyright (2004) Sandia Corporation
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
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
// @HEADER

#ifndef TEUCHOS_RCP_NODE_HPP
#define TEUCHOS_RCP_NODE_HPP


/** \file Teuchos_RCPNode.hpp
 *
 * \brief Reference-counted pointer node classes.
 */


#include "Teuchos_ConfigDefs.hpp"
#include "Teuchos_any.hpp"
#include "Teuchos_map.hpp"
#include "Teuchos_ENull.hpp"
#include "Teuchos_Assert.hpp"
#include "Teuchos_Exceptions.hpp"
#include "Teuchos_toString.hpp"


namespace Teuchos {


/** \brief Used to specify a pre or post destruction of extra data
 *
 * \ingroup teuchos_mem_mng_grp 
 */
enum EPrePostDestruction { PRE_DESTROY, POST_DESTROY };


/** \brief Used to specify if the pointer is weak or strong.
 *
 * \ingroup teuchos_mem_mng_grp 
 */
enum ERCPStrength { RCP_STRENGTH_INVALID=0, RCP_STRONG, RCP_WEAK };
// Above: Making RCP_STRENGTH_INVALID=0 helps to avoid problems with
// uninitialized memory that just happens to be zero! (see Code Complete: 2nd
// Edition)


template<>
class ToStringTraits<ERCPStrength> {
public:
  static std::string toString( const ERCPStrength &t )
    {
      switch (t) {
        case RCP_STRENGTH_INVALID:
          return "RCP_STRENGTH_INVALID";
        case RCP_STRONG:
          return "RCP_STRONG";
        case RCP_WEAK:
          return "RCP_STRONG";
        default:
#ifdef TEUCHOS_DEBUG
          TEST_FOR_EXCEPT(true);
#endif
          return "";
      }
      return ""; // Never be called!
    }
};



/** \brief Node class to keep track of address and the reference count for a
 * reference-counted utility class and delete the object.
 *
 * This is not a general user-level class.  This is used in the implementation
 * of all of the reference-counting utility classes.
 *
 * \ingroup teuchos_mem_mng_grp 
 */
class RCPNode {
public:
  /** \brief . */
  RCPNode(bool has_ownership_in)
    : strong_count_(1), weak_count_(0), has_ownership_(has_ownership_in),
      extra_data_map_(NULL)
    {}
  /** \brief . */
  virtual ~RCPNode()
    {
      if(extra_data_map_)
        delete extra_data_map_;
    }
  /** \brief . */
  int strong_count() const
    {
      return strong_count_; 
    }
  /** \brief . */
  int weak_count() const
    {
      return weak_count_; 
    }
  /** \brief . */
  int incr_count( const ERCPStrength strength )
    {
      switch (strength) {
        case RCP_STRONG:
          return ++strong_count_;
        case RCP_WEAK:
          return ++weak_count_;
        case RCP_STRENGTH_INVALID:
        default:
          TEST_FOR_EXCEPT(true);
      }
      return 0; // Never be called!
    }
  /** \brief . */
  int deincr_count( const ERCPStrength strength )
    {
      switch (strength) {
        case RCP_STRONG:
          return --strong_count_;
        case RCP_WEAK:
          return --weak_count_;
        case RCP_STRENGTH_INVALID:
        default:
          TEST_FOR_EXCEPT(true);
      }
      return 0; // Never be called!
    }
  /** \brief . */
  void has_ownership(bool has_ownership_in)
    {
      has_ownership_ = has_ownership_in;
    }
  /** \brief . */
  bool has_ownership() const
    {
      return has_ownership_;
    }
  /** \brief . */
  void set_extra_data(
    const any &extra_data, const std::string& name,
    EPrePostDestruction destroy_when, bool force_unique );
  /** \brief . */
  any& get_extra_data( const std::string& type_name,
    const std::string& name );
  /** \brief . */
  const any& get_extra_data( const std::string& type_name,
    const std::string& name
    ) const
    {
      return const_cast<RCPNode*>(this)->get_extra_data(type_name, name);
    }
  /** \brief . */
  any* get_optional_extra_data(const std::string& type_name,
    const std::string& name );
  /** \brief . */
  const any* get_optional_extra_data(
    const std::string& type_name, const std::string& name
    ) const
    {
      return const_cast<RCPNode*>(this)->get_optional_extra_data(type_name, name);
    }
  /** \brief . */
  virtual bool is_valid_ptr() const = 0;
  /** \brief . */
  virtual void delete_obj() = 0;
  /** \brief . */
  virtual void throw_invalid_obj_exception(
    const std::string& rcp_type_name,
    const void* rcp_ptr,
    const RCPNode* rcp_node_ptr,
    const void* rcp_obj_ptr
    ) const = 0;
protected:
  /** \brief . */
  void pre_delete_extra_data()
    {
      if(extra_data_map_)
        impl_pre_delete_extra_data();
    }
private:
  struct extra_data_entry_t {
    extra_data_entry_t() : destroy_when(POST_DESTROY) {}
    extra_data_entry_t( const any &_extra_data, EPrePostDestruction _destroy_when )
      : extra_data(_extra_data), destroy_when(_destroy_when)
      {}
    any extra_data;
    EPrePostDestruction destroy_when;
  }; 
  typedef Teuchos::map<std::string,extra_data_entry_t> extra_data_map_t;
  int strong_count_;
  int weak_count_;
  bool has_ownership_;
  extra_data_map_t *extra_data_map_;
  // Above is made a pointer to reduce overhead for the general case when this
  // is not used.  However, this adds just a little bit to the overhead when
  // it is used.
  // Provides the "basic" guarantee!
  void impl_pre_delete_extra_data();
  // Not defined and not to be called
  RCPNode();
  RCPNode(const RCPNode&);
  RCPNode& operator=(const RCPNode&);
};


/** \brief Implementation class for actually deleting the object.
 *
 * \ingroup teuchos_mem_mng_grp 
 */
template<class T, class Dealloc_T>
class RCPNodeTmpl : public RCPNode {
public:
  /** \brief . */
  RCPNodeTmpl(T* p, Dealloc_T dealloc, bool has_ownership_in)
    : RCPNode(has_ownership_in), ptr_(p),
#ifdef TEUCHOS_DEBUG
      deleted_ptr_(0),
#endif
      dealloc_(dealloc)
    {}
  /** \brief . */
  Dealloc_T& get_nonconst_dealloc()
    { return dealloc_; }
  /** \brief . */
  const Dealloc_T& get_dealloc() const
    { return dealloc_; }
  /** \brief . */
  ~RCPNodeTmpl()
    {
#ifdef TEUCHOS_DEBUG
      TEST_FOR_EXCEPTION( ptr_!=0, std::logic_error,
        "Error, the underlying object must be explicitly deleted before deleting"
        " the node object!" );
#endif
    }
  /** \brief . */
  virtual bool is_valid_ptr() const
    {
      return ptr_ != 0;
    }
  /** \brief Delete the underlying object.
   *
   * Provides the "strong guarantee" when exceptions are thrown in debug mode
   * and but may not even provide the "basic guarantee" in release mode.  .
   */
  virtual void delete_obj()
    {
      if (ptr_!= 0) {
        this->pre_delete_extra_data(); // May throw!
        T* tmp_ptr = ptr_;
#ifdef TEUCHOS_DEBUG
        deleted_ptr_ = tmp_ptr;
#endif
        ptr_ = 0;
        if (has_ownership()) {
#ifdef TEUCHOS_DEBUG
          try {
#endif
            dealloc_.free(tmp_ptr);
#ifdef TEUCHOS_DEBUG
          }
          catch(...) {
            // Object was not deleted due to an exception!
            ptr_ = tmp_ptr;
            throw;
          }
#endif
        }
        // 2008/09/22: rabartl: Above, we have to be careful to set the member
        // this->ptr_=0 before calling delete on the object's address in order
        // to avoid a double call to delete in cases of circular references
        // involving weak and strong pointers (see the unit test
        // circularReference_c_then_a in RCP_UnitTests.cpp).  NOTE: It is
        // critcial that no member of *this get accesses after
        // dealloc_.free(...) gets called!  Also, in order to provide the
        // "strong" guarantee we have to include the above try/catch.  This
        // overhead is unfortunate but I don't know of any other way to
        // statisfy the "strong" guarantee and still avoid a double delete.
      }
    }
  /** \brief . */
  virtual void throw_invalid_obj_exception(
    const std::string& rcp_type_name,
    const void* rcp_ptr,
    const RCPNode* rcp_node_ptr,
    const void* rcp_obj_ptr
    ) const
    {
      TEST_FOR_EXCEPT_MSG( ptr_!=0, "Internal coding error!" );
      const T* deleted_ptr =
#ifdef TEUCHOS_DEBUG
        deleted_ptr_
#else
        0
#endif
        ;
      TEST_FOR_EXCEPTION( true, DanglingReferenceError,
        "Error, an attempt has been made to dereference the underlying object\n"
        "from a weak smart pointer object where the underling object has already\n"
        "been deleted since the strong count has already gone to zero.\n"
        "\n"
        "Context information:\n"
        "\n"
        "  RCP type:             " << rcp_type_name << "\n"
        "  RCP address:          " << rcp_ptr << "\n"
        "  RCPNode type:         " << typeName(*this) << "\n"
        "  RCPNode address       " << rcp_node_ptr << "\n"
        "  RCP ptr address:      " << rcp_obj_ptr << "\n"
        "  Concrete ptr address: " << deleted_ptr << "\n"
        "\n"
        "Hint: Open your debugger and set conditional breakpoints in the various\n"
        "routines involved where this node object is first created with this\n"
        "concrete object and in all of the RCP objects of the type given above\n"
        "use this node object.  Debugging an error like this may take a little work\n"
        "setting up your debugging session but at least you don't have to try to\n"
        "track down a segfault that would occur otherwise!"
        );
      // 2008/09/22: rabartl: Above, we do not provide the concreate object
      // type or the concrete object address.  In the case of the concrete
      // object address, in a non-debug build, we don't want to pay a price
      // for extra storage that we strictly don't need.  In the case of the
      // concrete object type name, we don't want to force non-debug built
      // code to have the require that types be fully defined in order to use
      // the memory management software.  This is related to bug 4016.

    }
private:
  T *ptr_;
#ifdef TEUCHOS_DEBUG
  T *deleted_ptr_;
#endif
  Dealloc_T dealloc_;
  // not defined and not to be called
  RCPNodeTmpl();
  RCPNodeTmpl(const RCPNodeTmpl&);
  RCPNodeTmpl& operator=(const RCPNodeTmpl&);

}; // end class RCPNodeTmpl<T>


/** \brief Add new RCP to global list.
 *
 * \relates PrintActiveRCPNodes
 */
void add_new_RCPNode( RCPNode* rcp_node, const std::string &info );


/** \brief Remove RCP from global list.
 *
 * \relates PrintActiveRCPNodes
 */
void remove_RCPNode( RCPNode* rcp_node );


/** \brief Print global list on destruction.
 *
 * \ingroup teuchos_mem_mng_grp
 */
class PrintActiveRCPNodes {
public:
  /** \brief . */
  PrintActiveRCPNodes();
  /** \brief . */
  ~PrintActiveRCPNodes();
  /** \brief . */
  void foo();
private:
  static int count_;
};


/** \brief Return if we are tracing active nodes or not.
 *
 * NOTE: This will always return <tt>false</tt> when <tt>TEUCHOS_DEBUG</tt> is
 * not defined.
 *
 * \relates RCPNode
 */
bool isTracingActiveRCPNodes();


#ifdef TEUCHOS_DEBUG

/** \brief Set if we should be tracing active RCP nodes.
 *
 * This will only cause tracing of RCPNode-based objects that are created
 * after this has been called with <tt>true</tt>.  This function can later be
 * called with <tt>false</tt> to turn off tracing RCPNode objects.  This can
 * allow the client to keep track of RCPNode objects that get created in
 * specific blocks of code and can help as a debugging aid.
 *
 * NOTE: This function call will not even compile unless
 * <tt>TEUCHOS_DEBUG</tt> is defined!
 *
 * \relates RCPNode
 */
void setTracingActiveRCPNodes(bool tracingActiveNodes);

#endif // TEUCHOS_DEBUG


/** \brief Print the number of active RCPNode objects being tracked. */
int numActiveRCPNodes();


/** \brief Print the list of currently active RCP nodes.
 *
 * When the macro <tt>TEUCHOS_SHOW_ACTIVE_REFCOUNTPTR_NODE_TRACE</tt> is
 * defined, this function will print out all of the RCP nodes that are
 * currently active.  This function can be called at any time during a
 * program.
 *
 * When the macro <tt>TEUCHOS_SHOW_ACTIVE_REFCOUNTPTR_NODE_TRACE</tt> is
 * defined this function will get called automatically after the program ends
 * and all of the local and global RCP objects have been destroyed.  If any
 * RCP nodes are printed at that time, then this is an indication that there
 * may be some circular references that will caused memory leaks.  You memory
 * checking tool such as valgrind or purify should complain about this!
 *
 * \relates RCPNode
 */
void printActiveRCPNodes(std::ostream &out);


/** \brief Throw that a pointer passed into an RCP object is null. */
void throw_null_ptr_error( const std::string &type_name );


} // namespace Teuchos


namespace {
// This static variable should be delcared before all other static variables
// that depend on RCP or other classes. Therefore, this static varaible should
// be deleted *after* all of these other static variables that depend on RCP
// or created classes go away!
Teuchos::PrintActiveRCPNodes local_printActiveRCPNodes;
} // namespace


namespace Teuchos {


/** \brief Utility handle class for handling the reference counting and
 * managuement of the RCPNode object.
 *
 * Again, this is *not* a user-level class.  Instead, this class is used by
 * all of the user-level reference-counting classes.
 *
 * NOTE: I (Ross Bartlett) am not generally a big fan of handle classes and
 * greatly prefer smart pointers.  However, this is one case where a handle
 * class makes sense.  First, I want special behavior in some functions when
 * the wrapped RCPNode pointer is null.  Secound, I can't use one of the
 * smart-pointer classes because this class is used to implement all of those
 * smart-pointer classes!
 */
class RCPNodeHandle {
public:
  /** \brief . */
  RCPNodeHandle( ENull null_arg = null )
    : node_(0), strength_(RCP_STRENGTH_INVALID)
    {(void)null_arg;}
  /** \brief . */
  RCPNodeHandle( RCPNode* node, ERCPStrength strength_in = RCP_STRONG )
    : node_(node), strength_(strength_in)
    {}
#ifdef TEUCHOS_DEBUG
  /** \brief Only gets called in debug mode. */
  template<typename T>
  RCPNodeHandle( RCPNode* node, T *p, const std::string &T_name,
    const std::string &ConcreateT_name, const bool has_ownership_in,
    ERCPStrength strength_in = RCP_STRONG
    )
    : node_(node), strength_(strength_in)
    {
      TEUCHOS_ASSERT(strength_in == RCP_STRONG); // Can't handle yet!
      TEUCHOS_ASSERT(node_);
      if (isTracingActiveRCPNodes()) {
        std::ostringstream os;
        os << "{T=\'"<<T_name<<"\',Concrete T=\'"
           <<ConcreateT_name<<"\',p="<<p<<",has_ownership="<<has_ownership_in<<"}";
        add_new_RCPNode(node_, os.str());
      }
    }
#endif // TEUCHOS_DEBUG
  /** \brief . */
  RCPNodeHandle( const RCPNodeHandle& node_ref )
    : node_(node_ref.node_), strength_(node_ref.strength_)
    {
      if (node_)
        node_->incr_count(strength_);
    }
  /** \brief (Strong guarantee). */
  RCPNodeHandle& operator=( const RCPNodeHandle& node_ref )
    {
      // Assignment to self check
      if ( this == &node_ref )
        return *this;
      // Take care of this's existing node and object
      unbind(); // May throw in some cases
      // Assign the new node
      node_ = node_ref.node_;
      strength_ = node_ref.strength_;
      if (node_)
        node_->incr_count(strength_);
      return *this;
    }
  /** \brief . */
  ~RCPNodeHandle()
    {
      unbind();
    }
  /** \brief . */
  RCPNodeHandle create_weak() const
    {
      if (node_) {
        node_->incr_count(RCP_WEAK);
        return RCPNodeHandle(node_, RCP_WEAK);
      }
      return RCPNodeHandle();
    }
  /** \brief . */
  RCPNode* node_ptr() const
    {
      return node_;
    }
  /** \brief . */
  bool is_node_null() const
    {
      return node_==0;
    }
  /** \brief . */
  bool is_valid_ptr() const
    {
      if (node_)
        return node_->is_valid_ptr();
      return true; // Null is a valid ptr!
    }
  /** \brief . */
  bool same_node(const RCPNodeHandle &node2) const
    {
      return node_ == node2.node_;
    }
  /** \brief . */
  int strong_count() const
    {
      if (node_)
        return node_->strong_count(); 
      return 0;
    }
  /** \brief . */
  int weak_count() const
    {
      if (node_)
        return node_->weak_count(); 
      return 0;
    }
  /** \brief . */
  int total_count() const
    {
      if (node_)
        return node_->strong_count() + node_->weak_count(); 
      return 0;
    }
  /** \brief Backward compatibility. */
  int count() const
    {
      if (node_)
        return node_->strong_count(); 
      return 0;
    }
  /** \brief . */
  ERCPStrength strength() const
    {
      return strength_;
    }
  /** \brief . */
  void has_ownership(bool has_ownership_in)
    {
      if (node_)
        node_->has_ownership(has_ownership_in);
    }
  /** \brief . */
  bool has_ownership() const
    {
      if (node_)
        return node_->has_ownership();
      return false;
    }
  /** \brief . */
  void set_extra_data(
    const any &extra_data, const std::string& name,
    EPrePostDestruction destroy_when, bool force_unique
    )
    {
      debug_assert_not_null();
      node_->set_extra_data(extra_data, name, destroy_when, force_unique);
    }
  /** \brief . */
  any& get_extra_data( const std::string& type_name,
    const std::string& name
    )
    {
      debug_assert_not_null();
      return node_->get_extra_data(type_name, name);
    } 
  /** \brief . */
  const any& get_extra_data( const std::string& type_name,
    const std::string& name 
    ) const
    {
      return const_cast<RCPNodeHandle*>(this)->get_extra_data(type_name, name);
    }
  /** \brief . */
  any* get_optional_extra_data(
    const std::string& type_name, const std::string& name
    )
    {
      debug_assert_not_null();
      return node_->get_optional_extra_data(type_name, name);
    } 
  /** \brief . */
  const any* get_optional_extra_data(
    const std::string& type_name, const std::string& name
    ) const
    {
      return const_cast<RCPNodeHandle*>(this)->get_optional_extra_data(type_name, name);
    }
  /** \brief . */
  void debug_assert_not_null() const
    {
#ifdef TEUCHOS_DEBUG
      if (!node_)
        throw_null_ptr_error(typeName(*this));
#endif
    }
  /** \brief . */
  template<class RCPType>
  void assert_valid_ptr(const RCPType& rcp_obj) const
    {
      if (!node_)
        return; // Null is a valid pointer!
      if (!is_valid_ptr()) {
        node_->throw_invalid_obj_exception( TypeNameTraits<RCPType>::name(),
          this, node_, rcp_obj.access_private_ptr() );
      }
    }
  /** \brief . */
  template<class RCPType>
  void debug_assert_valid_ptr(const RCPType& rcp_obj) const
    {
#ifdef TEUCHOS_DEBUG
      assert_valid_ptr(rcp_obj);
#endif
    }
private:
  RCPNode *node_;
  ERCPStrength strength_;
  // Provides the "strong" guarantee!
  void unbind()
    {
      if (node_) {
        // NOTE: We only deincrement the reference count after
        // we have called delete on the underlying object since
        // that call to delete may actually thrown an exception!
        if (node_->strong_count()==1 && strength()==RCP_STRONG) {
          // Delete the object (which might throw)
          node_->delete_obj();
        }
        // If we get here, no exception was thrown!
        if ( (node_->strong_count() + node_->weak_count()) == 1 ) {
          // The last RCP object is going away so time to delete
          // the entire node!
#ifdef TEUCHOS_DEBUG
          local_printActiveRCPNodes.foo(); // Make sure created!
          remove_RCPNode(node_);
#endif
          delete node_;
          node_ = 0;
          // NOTE: No need to deincrement the reference count since this is
          // the last RCP object being deleted!
        }
        else {
          // The last RCP has not gone away so just deincrement the reference
          // count.
          node_->deincr_count(strength());
        }
      }
    }
};


/** \brief Ouput stream operator for RCPNodeHandle.
 *
 * \relates RCPNodeHandle
 */
inline
std::ostream& operator<<(std::ostream& out, const RCPNodeHandle& node)
{
  out << node.node_ptr();
  return out;
}


} // end namespace Teuchos


#endif // TEUCHOS_RCP_NODE_HPP
