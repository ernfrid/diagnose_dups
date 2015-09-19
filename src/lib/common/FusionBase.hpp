#pragma once

#include <boost/fusion/include/less.hpp>
#include <boost/fusion/include/equal_to.hpp>
#include <boost/fusion/include/hash.hpp>

#include <cstddef>

// This class inherits from StorageType, which is expected to be a boost
// fusion sequence. Typically, in this project, this will mean a struct
// defined with BOOST_FUSION_DEFINE_STRUCT_INLINE. This provides us with
// a fusion sequence over the data members while also allowing us to
// access them like an ordinary struct.
//
// The second template parameter, DerivedType, is a CRTP parameter.
// Briefly, DerivedType is the type of a child class that is inheriting
// from this thing, e.g.,
//
//      BOOST_FUSION_DEFINE_STRUCT_INLINE(
//          Whee,
//          (std::string, name)
//          (int x)
//          (int y)
//      )
//
//      struct Whee : FusionBase<WheeStorage, Whee> { ... };
//
// This lets us define common functions for DerivedType (Whee!) that make
// sense across all types of heterogeneous sequences just once, right here
// in this template (e.g., comparison operators, hash functions, ...).
template<typename StorageType, typename DerivedType>
struct FusionBase : StorageType {

#if (__cplusplus >= 201103L)
    // C++11 constructor, takes any number of arguments and forwards them
    // to the storage type. boost::fusion sequences provide a constructor
    // that does element-wise initialization in declaration order.
    template<typename... Xs>
    FusionBase(Xs&&... xs)
        : StorageType(std::forward<Xs>(xs)...)
    {}
#endif
    // NOTE: if C++11 is not enabled, children of FusionBase<S, D> will
    // have to initialize the data members of S in the BODY of their
    // constructors (if they wish to do any initialization at all that
    // is). This is because initializer lists can not be used to
    // directly initialize members of base classes.

    StorageType const& raw_storage() const {
        return *static_cast<StorageType const*>(this);
    }

    friend bool operator<(DerivedType const& x, DerivedType const& y) {
        return x.raw_storage() < y.raw_storage();
    }

    friend bool operator==(DerivedType const& x, DerivedType const& y) {
        return x.raw_storage() == y.raw_storage();
    }

    friend std::size_t hash_value(DerivedType const& x) {
        return boost::fusion::hash_value(x);
    }

    // add more generic operators that work on heterogeneous sequences
    // here...
};
