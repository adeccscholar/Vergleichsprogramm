#pragma once

#include <string>
#include <string_view>
#include <functional>

using namespace std::literals;

/**
 * @brief Address or cadastral data
 * Gemeinde commune
 * Kreis district / country
 * Bundesland federal land / state (German State)
 * Stadtteil quarter
*/
class TAddress {
   friend std::ostream& operator << (std::ostream& out, TAddress const& data) {
      out << data.strZipCode << (data.strZipCode.length() > 0 ? " "s : ""s) << data.strCity
          << ", "s << data.strStreet << (data.strStreetNumber.length() > 0 ? " "s : ""s)
          << data.strStreetNumber;
      return out;
      }
   friend void swap(TAddress& lhs, TAddress& rhs) noexcept { lhs.swap(rhs); }
private:
   std::string strCity = "";           ///< Stadt, an der sich die Adresse befindet
   std::string strStreet = "";         ///< Straße zu dieser Adresse
   std::string strStreetNumber = "";   ///< Hausnummer zu dieser Straße
   std::string strZipCode = "";        ///< Postleitzahl
   std::string strUrbanUnit = "";      ///< Stadtbezirk
   std::string strUrbanUnit_Old = "";  ///< Stadtbezirk (alte Aufteilung)
   std::string strDistrict = "";       ///< Ortsteil

public:
   TAddress(void) {
      _init();
      }

   TAddress(TAddress const& ref) {
      _copy(ref);
      }

   TAddress(TAddress&& ref) noexcept {
      swap(ref);
      }


   virtual ~TAddress(void) = default;

   virtual TAddress* create() { return new TAddress; }

   TAddress& operator = (TAddress const& ref) {
      copy(ref);
      return *this;
   }

   TAddress& operator = (TAddress&& ref) noexcept {
      swap(ref);
      return *this;
   }

   void swap(TAddress& ref) noexcept {
      using std::swap;
      swap(strCity,          ref.strCity);
      swap(strStreet,        ref.strStreet);
      swap(strStreetNumber,  ref.strStreetNumber);
      swap(strZipCode,       ref.strZipCode);
      swap(strUrbanUnit,     ref.strUrbanUnit);
      swap(strUrbanUnit_Old, ref.strUrbanUnit_Old);
      swap(strDistrict,      ref.strDistrict);
      }

   virtual void init() { _init(); }
   virtual void copy(TAddress const& ref) { _copy(ref); }


   /// /name Selektoren
   /// {
   std::string const& City(void) const { return strCity; }
   std::string const& Street(void) const { return strStreet; }
   std::string const& StreetNumber(void) const { return strStreetNumber; }
   std::string const& ZipCode(void) const { return strZipCode; }
   std::string const& UrbanUnit(void) const { return strUrbanUnit; }
   std::string const& UrbanUnit_Old(void) const { return strUrbanUnit_Old; }
   std::string const& District(void) const { return strDistrict; }
   /// }

   void City(std::string const& newVal) { strCity = newVal; }
   void Street(std::string const& newVal) { strStreet = newVal; }
   void StreetNumber(std::string const& newVal) { strStreetNumber = newVal; }
   void ZipCode(std::string const& newVal) { strZipCode = newVal; }
   void UrbanUnit(std::string const& newVal) { strUrbanUnit = newVal; }
   void UrbanUnit_Old(std::string const& newVal) { strUrbanUnit_Old = newVal; }
   void District(std::string const& newVal) { strDistrict = newVal; }

   void City(std::string&& newVal) { strCity = std::forward<std::string>(newVal); }
   void Street(std::string&& newVal) { strStreet = std::forward<std::string>(newVal); }
   void StreetNumber(std::string&& newVal) { strStreetNumber = std::forward<std::string>(newVal); }
   void ZipCode(std::string&& newVal) { strZipCode = std::forward<std::string>(newVal); }
   void UrbanUnit(std::string&& newVal) { strUrbanUnit = std::forward<std::string>(newVal); }
   void UrbanUnit_Old(std::string&& newVal) { strUrbanUnit_Old = std::forward<std::string>(newVal); }
   void District(std::string&& newVal) { strDistrict = std::forward<std::string>(newVal); }

   /// Manipulatoren 
   /*
   void City(std::string_view const& newVal) { strCity = std::move(std::string{ newVal.data(), newVal.size() }); }
   void Street(std::string_view const& newVal) { strStreet = std::move(std::string{ newVal.data(), newVal.size() }); }
   void StreetNumber(std::string_view const& newVal) { strStreetNumber = std::move(std::string{ newVal.data(), newVal.size() }); }
   void ZipCode(std::string_view const& newVal) { strZipCode = std::move(std::string{ newVal.data(), newVal.size() }); }
   void UrbanUnit(std::string_view const& newVal) { strUrbanUnit = std::move(std::string{ newVal.data(), newVal.size() }); }
   void UrbanUnit_Old(std::string_view const& newVal) { strUrbanUnit_Old = std::move(std::string{ newVal.data(), newVal.size() }); }
   void District(std::string_view const& newVal) { strDistrict = std::move(std::string{ newVal.data(), newVal.size() }); }
   */
   void City(std::string_view newVal) { strCity =  newVal ; }
   void Street(std::string_view newVal) { strStreet = newVal; }
   void StreetNumber(std::string_view newVal) { strStreetNumber = newVal; }
   void ZipCode(std::string_view newVal) { strZipCode = newVal; }
   void UrbanUnit(std::string_view newVal) { strUrbanUnit = newVal; }
   void UrbanUnit_Old(std::string_view newVal) { strUrbanUnit_Old = newVal; }
   void District(std::string_view newVal) { strDistrict = newVal; }

private:
   void _init(void) {
      strCity = "";
      strStreet = "";
      strStreetNumber = "";
      strZipCode = "";
      strUrbanUnit = "";
      strUrbanUnit_Old = "";
      strDistrict = "";
   }
   void _copy(TAddress const& ref) {
      City(ref.City());
      Street(ref.Street());
      StreetNumber(ref.StreetNumber());
      ZipCode(ref.ZipCode());
      UrbanUnit(ref.UrbanUnit());
      UrbanUnit_Old(ref.UrbanUnit_Old());
      District(ref.District());
   }

};
