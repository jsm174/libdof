#pragma once

#include "../../../general/generic/NamedItemBase.h"
#include "../../../general/bitmap/FastBitmap.h"
#include "../../../general/bitmap/FastBitmapDataExtractModeEnum.h"

namespace DOF
{

class Shape : public NamedItemBase
{
private:
   int m_bitmapFrameNumber;
   int m_bitmapTop;
   int m_bitmapLeft;
   int m_bitmapWidth;
   int m_bitmapHeight;
   FastBitmapDataExtractModeEnum m_dataExtractMode;

public:
   Shape();
   virtual ~Shape();

   int GetBitmapFrameNumber() const;
   void SetBitmapFrameNumber(int value);

   int GetBitmapTop() const;
   void SetBitmapTop(int value);

   int GetBitmapLeft() const;
   void SetBitmapLeft(int value);

   int GetBitmapWidth() const;
   void SetBitmapWidth(int value);

   int GetBitmapHeight() const;
   void SetBitmapHeight(int value);

   FastBitmapDataExtractModeEnum GetDataExtractMode() const;
   void SetDataExtractMode(FastBitmapDataExtractModeEnum value);
};

}