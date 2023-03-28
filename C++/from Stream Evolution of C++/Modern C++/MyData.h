#pragma once

#include "MyAddress.h"
#include "MyLocation.h"
#include "Tool_Helper.h"

#include <iomanip>
#include <algorithm>
#include <charconv>
#include <vector>
#include <execution>

template <typename ty>
class TData : public TAddress {
   friend void swap(TData& lhs, TData& rhs) noexcept { lhs.swap(rhs); }
private:
   Location<ty> mLoc;
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
};

template <typename ty>
using data_vector = std::vector<std::pair<TData<ty>, Result<ty>>>;

using func_vector = std::vector<std::function<void(TData<double>&, std::string const&)>>;

using func_vector_vw = std::vector<std::function<void(TData<double>&, std::string_view const&)>>;


template <typename ty>
inline void Write(typename data_vector<ty>::const_iterator begin, typename data_vector<ty>::const_iterator end, std::ostream& os) {
   //os.setf(std::ios::showpoint);
   //os.setf(std::ios::fixed);
   //os.precision(6);
   /*
   std::for_each(vData.cbegin(), vData.cend(), [&os](auto const& val) {
      os << val.first.ZipCode() << " " << val.first.City() << " / " << val.first.UrbanUnit() << ", "
         << val.first.Street() << " " << val.first.StreetNumber() 
         << " -> (" << std::setprecision(6) << val.first.Latitude() << ", "
         << std::setprecision(6) << val.first.Longitude() << ") -> " 
         << std::setprecision(3) << val.second.first << "m in "
         << std::setprecision(1) << val.second.second << "°\n";
   */
   
   /*
   std::for_each(begin, end, [&os](auto const& val) {
      os << val.first.ZipCode() << " " << val.first.City() << " / " << val.first.UrbanUnit() << ", "
         << val.first.Street() << " " << val.first.StreetNumber()
         << " -> (" << my_Double_to_String_G(val.first.Latitude(), 6) << ", "
         << my_Double_to_String_G(val.first.Longitude(), 6) << ") -> "
         << my_Double_to_String_G(val.second.first, 3) << "m in "
         << my_Double_to_String_G(val.second.second, 1) << "°\n";
      });
   */
   ///*
   std::string strBuffer;
   std::for_each(begin, end, [&strBuffer](auto const& val) {
      auto const& [address, result] = val;
      std::format_to(std::back_inserter(strBuffer), "{};{};{};{};{};{:.9f};{:.9f};{:.3f};{:.1f}\n",
         address.ZipCode(), address.City(), address.UrbanUnit(), address.Street(), address.StreetNumber(),
         address.Latitude(), address.Longitude(), result.first, result.second);
      });
   os.write(strBuffer.data(), strBuffer.size());
   //*/
}


template <typename ty>
inline void Write(data_vector<ty> const& vData, std::ostream& os) {
   Write<ty>(vData.cbegin(), vData.cend(), os);
   }

template< typename ty>
inline void Calculate(Location<ty> const& pointA, typename data_vector<ty>::iterator begin, typename data_vector<ty>::iterator end) {
   static constexpr auto my_round = [](double const& val) {
      return std::round(val * 1000.0) / 1000.0;
   };

   static const double r = 6371000.785;                     //< mean radius of the earth
#if defined __BORLANDC__
   static const double w1 = M_PI / 180.0;
   static const double w2 = 180.0 / M_PI;
#else
   static constexpr double pi = 3.14159265358979323846;
   static const double w1 = pi / 180.0;
   static const double w2 = 180.0 / pi;
#endif
   double phiA = pointA.first * w1; /// 180.0 * M_PI;
   double lambdaA = pointA.second * w1; /// 180.0 * M_PI;

#if defined __BORLANDC__
   for_each(begin, end, [&phiA, &lambdaA](auto& val) mutable {
#else
   for_each(std::execution::par, begin, end, [&phiA, &lambdaA](auto& val) mutable {
#endif
      Location<ty> const& pointB = val.first;
      double phiB = pointB.first * w1; /// 180.0 * M_PI;
      double lambdaB = pointB.second * w1; /// 180.0 * M_PI;
      double zeta = std::acos(std::sin(phiA) * std::sin(phiB) + std::cos(phiA) * std::cos(phiB) * std::cos(lambdaB - lambdaA));
      double alpha = std::acos((std::sin(phiB) - std::sin(phiA) * std::cos(zeta)) / (std::cos(phiA) * std::sin(zeta)));
      if (std::isnan(alpha)) {
         alpha = (std::sin(phiB) - std::sin(phiA) * std::cos(zeta)) / (std::cos(phiA) * std::sin(zeta)) < -1.0 ? 180.0 : 0.0;
         }
      else alpha = my_round(alpha * w2);
      val.second = std::make_pair(my_round(zeta * r), lambdaA > lambdaB ? phiA > 0 ? 360.0 - alpha : 180.0 + alpha : phiA > 0 ? alpha : 180.0 - alpha);
      });
}