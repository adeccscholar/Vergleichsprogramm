#pragma once

#include "MyAddress.h"
#include "MyLocation.h"
#include "Tool_Helper.h"

#include <iomanip>
#include <concepts>
#include <algorithm>
#include <charconv>
#include <vector>
#include <execution>


template <typename ty> requires std::floating_point<ty>
class TData : public TAddress {
   friend void swap(TData& lhs, TData& rhs) noexcept { lhs.swap(rhs); }
private:
   Location<ty> mLoc;
public:
   TData(void) : TAddress() { _init(); }
   TData(TData const& ref) : TAddress(ref) { _copy(ref); }
   TData(TAddress const& ref) : TAddress(ref) { _init(); }
   TData(TData&& ref) noexcept : TAddress(std::forward<TAddress>(ref)) { _swap(ref); }

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
      _swap(ref);
      }

   virtual TAddress* create() override { return new TData<ty>; }
   virtual void init(void) override { TAddress::init();  _init(); };
   virtual void copy(TAddress const& ref) override {
      TAddress::copy(ref);
      try {
         _copy(dynamic_cast<TData const&>(ref));
         }
      catch (std::bad_cast&) { ; }
   }

   operator Location<ty>& () { return mLoc; }
   operator Location<ty> const& () const { return mLoc; }

   ty const& Latitude(void) const { return mLoc.first; }
   ty const& Longitude(void) const { return mLoc.second; }


   void Latitude(ty const& newVal) { mLoc.first = newVal; }
   void Longitude(ty const& newVal) { mLoc.second = newVal; }

   void Latitude(std::string const& newVal) { auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), mLoc.first); }  // mLoc.first = std::stod(newVal); }
   void Longitude(std::string const& newVal) { auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), mLoc.second); } //mLoc.second = std::stod(newVal); }

   void Latitude(std::string&& newVal) { auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), mLoc.first); }  // mLoc.first = std::stod(newVal); }
   void Longitude(std::string&& newVal) { auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), mLoc.second); } //mLoc.second = std::stod(newVal); }

   void Latitude(std::string_view const& newVal) { auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), mLoc.first); }  // mLoc.first = std::stod(newVal); }
   void Longitude(std::string_view const& newVal) { auto result = std::from_chars(newVal.data(), newVal.data() + newVal.size(), mLoc.second); } //mLoc.second = std::stod(newVal); }

private:
   void _init(void) { mLoc = { 0.0, 0.0 }; }
   void _copy(TData const& ref) { mLoc = ref.mLoc; }

   void _swap(TData& ref) noexcept {
      std::swap(mLoc, ref.mLoc);
   }

};


template <typename ty>
requires std::floating_point<ty>
using data_vector = std::vector<std::pair<TData<ty>, Result<ty>>>;

template <typename ty>
requires std::floating_point<ty>
using func_vector = std::vector<std::function<void(TData<ty>&, std::string const&)>>;

template <typename ty>
requires std::floating_point<ty>
using func_vector_vw = std::vector<std::function<void(TData<ty>&, std::string_view const&)>>;


template< typename ty>
requires std::floating_point<ty>
inline void Calculate(Location<ty> const& pointA, typename data_vector<ty>::iterator begin, typename data_vector<ty>::iterator end) {
   static constexpr auto my_round = [](ty const& val) {
      return std::round(val * 1000.0) / 1000.0;
   };

   static const ty r  = 6371000.785;                     //< mean radius of the earth
   static const ty w1 = std::numbers::pi_v<ty> / 180.0;
   static const ty w2 = 180.0 / std::numbers::pi_v<ty>;

   const ty phiA    = pointA.first  * w1; /// 180.0 * M_PI;
   const ty lambdaA = pointA.second * w1; /// 180.0 * M_PI;
   const ty sin_phiA = std::sin(phiA);
   const ty cos_phiA = std::cos(phiA);

   for_each(std::execution::par, begin, end, [&phiA, &lambdaA, sin_phiA, cos_phiA](auto& val) mutable {
      ty phiB    = val.first.Latitude() * w1;  /// 180.0 * M_PI;
      ty lambdaB = val.first.Longitude() * w1; /// 180.0 * M_PI;
      ty zeta = std::acos(sin_phiA * std::sin(phiB) + cos_phiA * std::cos(phiB) * std::cos(lambdaB - lambdaA));
      ty tmp = (std::sin(phiB) - sin_phiA * std::cos(zeta)) / (cos_phiA * std::sin(zeta));
      ty alpha = tmp < -1 ? 180.0 : tmp > 1 ? 0 : std::acos(tmp) * w2;

      /*
      double alpha = std::acos((std::sin(phiB) - std::sin(phiA) * std::cos(zeta)) / (std::cos(phiA) * std::sin(zeta)));
      if (std::isnan(alpha)) {
         alpha = (std::sin(phiB) - std::sin(phiA) * std::cos(zeta)) / (std::cos(phiA) * std::sin(zeta)) < -1.0 ? 180.0 : 0.0;
         }
      else alpha = my_round(alpha * w2);
      */
      val.second = std::make_pair(my_round(zeta * r), 
                                  lambdaA>lambdaB ? phiA>0 ? 360.0-alpha : 180.0+alpha : phiA>0 ? alpha : 180.0-alpha);
      });
}