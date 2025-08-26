#pragma once

#include "IMatrixEffect.h"
#include "../../general/FilePattern.h"
#include "../../general/bitmap/FastBitmapDataExtractModeEnum.h"

namespace DOF
{

class IMatrixBitmapEffect : public virtual IMatrixEffect
{
public:
   virtual ~IMatrixBitmapEffect() = default;

   virtual FilePattern* GetBitmapFilePattern() const = 0;
   virtual void SetBitmapFilePattern(FilePattern* value) = 0;

   virtual int GetBitmapFrameNumber() const = 0;
   virtual void SetBitmapFrameNumber(int value) = 0;

   virtual int GetBitmapHeight() const = 0;
   virtual void SetBitmapHeight(int value) = 0;

   virtual int GetBitmapLeft() const = 0;
   virtual void SetBitmapLeft(int value) = 0;

   virtual int GetBitmapTop() const = 0;
   virtual void SetBitmapTop(int value) = 0;

   virtual int GetBitmapWidth() const = 0;
   virtual void SetBitmapWidth(int value) = 0;

   virtual FastBitmapDataExtractModeEnum GetDataExtractMode() const = 0;
   virtual void SetDataExtractMode(FastBitmapDataExtractModeEnum value) = 0;
};

}