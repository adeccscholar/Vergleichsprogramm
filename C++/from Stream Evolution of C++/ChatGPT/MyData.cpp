#include "MyData.h"

TData::TData(void) : TAddress() { 
   _init(); 
   }

TData::TData(TData const& ref) : TAddress(ref) { 
   _copy(ref); 
   }

#if defined WITH_MOVE
TData::TData(TData&& ref) noexcept : TAddress(ref) { 
   swap(ref); 
   }
#endif

TData& TData::operator = (TData const& ref) {
   copy(ref);
   return *this;
   }

#if defined WITH_MOVE   
TData& TData::operator = (TData&& ref) noexcept {
   swap(ref);
   return *this;
   }

void TData::swap(TData& ref) noexcept {
   TAddress::swap(static_cast<TAddress&>(ref));
   using std::swap;
   swap(mLoc, ref.mLoc);
   }
#endif

TAddress* TData::create() { 
   return new TData; 
   }

void TData::init(void) { 
   TAddress::init();  
   _init(); 
   }

void TData::copy(TAddress const& ref) {
   TAddress::copy(ref);
   try {
      _copy(dynamic_cast<TData const&>(ref));
      }
   catch (std::bad_cast&) { ; }
   }

void TData::_init(void) { 
   mLoc = { 0.0, 0.0 }; 
   }

void TData::_copy(TData const& ref) { 
   mLoc = ref.mLoc; 
   }
