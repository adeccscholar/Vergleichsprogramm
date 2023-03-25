#pragma once

#include "MyAddress.h"
#include "MyLocation.h"
#include "Tool_Helper.h"

#include <iomanip>
#include <algorithm>
#include <charconv>
#include <vector>
#include <execution>

class TData : public TAddress {
   friend void swap(TData& lhs, TData& rhs) noexcept { lhs.swap(rhs);  }
private:
   Location mLoc;
public:
   TData(void) : TAddress() { _init(); }
   TData(TData const& ref) : TAddress(ref) { _copy(ref); }

   TData(TData&& ref) noexcept : TAddress(ref) { swap(ref); }

   virtual ~TData(void) override { }

   TData& operator = (TData const& ref) {
      copy(ref);
      return *this;
   }

   
   TData& operator = (TData&& ref) noexcept {
      swap(ref);
      return *this;
      }
   

   void swap(TData& ref) {
      TAddress::swap(static_cast<TAddress&>(ref));
      using std::swap;
      swap(mLoc, ref.mLoc);
      }

   virtual TAddress* create() override { return new TData; }
   virtual void init(void) override { TAddress::init();  _init(); };
   virtual void copy(TAddress const& ref) override {
      TAddress::copy(ref);
      try {
         _copy(dynamic_cast<TData const&>(ref));
      }
      catch (std::bad_cast&) { ; }
   }

   operator Location& () { return mLoc; }
   operator Location const& () const { return mLoc; }

   double const& Latitude(void) const { return mLoc.Latitude(); }
   double const& Longitude(void) const { return mLoc.Longitude(); }


   void Latitude(double const& newVal) { mLoc.Latitude(newVal); }
   void Longitude(double const& newVal) { mLoc.Longitude(newVal); }

private:
   void _init(void) { mLoc = { 0.0, 0.0 }; }
   void _copy(TData const& ref) { mLoc = ref.mLoc; }
};

using data_vector = std::vector<std::pair<TData, Result>>;

