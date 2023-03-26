#pragma once

#include "MyAddress.h"
#include "MyLocation.h"

#include <iomanip>
#include <vector>

class TData : public TAddress {
#if defined WITH_MOVE
   friend void swap(TData& lhs, TData& rhs) noexcept { lhs.swap(rhs);  }
#endif
private:
   Location mLoc;
public:
   TData(void);
   TData(TData const& ref);
#if defined WITH_MOVE
   TData(TData&& ref) noexcept;
#endif
   virtual ~TData(void) override { }
   TData& operator = (TData const& ref);

#if defined WITH_MOVE   
   TData& operator = (TData&& ref) noexcept;
   void swap(TData& ref) noexcept;
#endif

   virtual TAddress* create() override;
   virtual void init(void) override;
   virtual void copy(TAddress const& ref) override;

   operator Location& () { return mLoc; }
   operator Location const& () const { return mLoc; }

   double const& Latitude(void) const { return mLoc.Latitude(); }
   double const& Longitude(void) const { return mLoc.Longitude(); }


   void Latitude(double const& newVal) { mLoc.Latitude(newVal); }
   void Longitude(double const& newVal) { mLoc.Longitude(newVal); }

private:
   void _init(void);
   void _copy(TData const& ref);
};

using data_vector = std::vector<std::pair<TData, Result>>;

