#pragma once

#include <string>
#include <string_view>
#include <functional>

//#define WITH_MOVE

/**
 * @brief Address or cadastral data
*/
class TAddress {
#if defined WITH_MOVE
   friend void swap(TAddress& lhs, TAddress& rhs) noexcept { lhs.swap(rhs); }
#endif
private:
   std::string strCity = "";  ///< Stadt, an der sich die Adresse befindet
   std::string strStreet = "";  ///< Straße zu dieser Adresse
   std::string strStreetNumber = "";  ///< Hausnummer zu dieser Straße
   std::string strZipCode = "";  ///< Postleitzahl
   std::string strUrbanUnit = "";  ///< Stadtbezirk
   std::string strUrbanUnit_Old = "";  ///< Stadtbezirk (alte Aufteilung)
   std::string strDistrict = "";  ///< Ortsteil

public:
   TAddress(void) {
      _init();
   }

   TAddress(TAddress const& ref) {
      _copy(ref);
   }

#if defined WITH_MOVE
   TAddress(TAddress&& ref) noexcept {
      swap(ref);
   }
#endif   

   virtual ~TAddress(void) = default;

   virtual TAddress* create();

   TAddress& operator = (TAddress const& ref);
#if defined WITH_MOVE
   TAddress& operator = (TAddress&& ref) noexcept;
   void swap(TAddress& ref) noexcept;
#endif
   virtual void init();
   virtual void copy(TAddress const& ref);

   // Compare the Addresses with DIN 5007-2 Norm, Streetnumber with numeric part and addition
   bool CompareDIN5007(TAddress const& other) const;

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

private:
   void _init(void);
   void _copy(TAddress const& ref);
 
   static std::string normalizeDIN(std::string const& strText);
   static void Parse_StreetNumber(std::string const& strInput, int& streetnumber, std::string& addition);
};
