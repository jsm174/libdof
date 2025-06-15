#pragma once

#include "MatrixEffectBase.h"
#include "../RetriggerBehaviourEnum.h"
#include "../../general/bitmap/FastBitmap.h"
#include "../../general/MathExtensions.h"
#include "../../general/StringExtensions.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../Log.h"

namespace DOF
{

class TableElementData;

template <typename MatrixElementType> class MatrixBitmapEffectBase : public MatrixEffectBase<MatrixElementType>
{
public:
   MatrixBitmapEffectBase();
   virtual ~MatrixBitmapEffectBase() = default;

   const std::string& GetBitmapFilePattern() const { return m_bitmapFilePattern; }
   void SetBitmapFilePattern(const std::string& value);
   int GetBitmapFrameNumber() const { return m_bitmapFrameNumber; }
   void SetBitmapFrameNumber(int value) { m_bitmapFrameNumber = MathExtensions::Limit(value, 0, 1000); }
   int GetBitmapLeft() const { return m_bitmapLeft; }
   void SetBitmapLeft(int value) { m_bitmapLeft = MathExtensions::Limit(value, 0, 1000); }
   int GetBitmapTop() const { return m_bitmapTop; }
   void SetBitmapTop(int value) { m_bitmapTop = MathExtensions::Limit(value, 0, 1000); }
   int GetBitmapWidth() const { return m_bitmapWidth; }
   void SetBitmapWidth(int value) { m_bitmapWidth = MathExtensions::Limit(value, -1, 1000); }
   int GetBitmapHeight() const { return m_bitmapHeight; }
   void SetBitmapHeight(int value) { m_bitmapHeight = MathExtensions::Limit(value, -1, 1000); }
   RetriggerBehaviourEnum GetRetriggerBehaviour() const { return m_retriggerBehaviour; }
   void SetRetriggerBehaviour(RetriggerBehaviourEnum value) { m_retriggerBehaviour = value; }
   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Init(Table* table) override;
   virtual void Finish() override;

protected:
   virtual MatrixElementType GetInactiveValue() = 0;
   virtual MatrixElementType GetPixelValue(const PixelData& pixel, int triggerValue) = 0;
   void RenderBitmap(TableElementData* tableElementData);
   void LoadBitmap();
   PixelData GetScaledPixel(int x, int y) const;

   FastBitmap m_bitmap;
   bool m_bitmapLoaded;
   std::string m_bitmapFilePattern;

private:
   int m_bitmapFrameNumber;
   int m_bitmapLeft;
   int m_bitmapTop;
   int m_bitmapWidth;
   int m_bitmapHeight;

protected:
   RetriggerBehaviourEnum m_retriggerBehaviour;
   bool m_active;
};


template <typename MatrixElementType>
MatrixBitmapEffectBase<MatrixElementType>::MatrixBitmapEffectBase()
   : m_bitmapLoaded(false)
   , m_bitmapFrameNumber(0)
   , m_bitmapLeft(0)
   , m_bitmapTop(0)
   , m_bitmapWidth(-1)
   , m_bitmapHeight(-1)
   , m_retriggerBehaviour(RetriggerBehaviourEnum::Restart)
   , m_active(false)
{
}

template <typename MatrixElementType> void MatrixBitmapEffectBase<MatrixElementType>::SetBitmapFilePattern(const std::string& value)
{
   if (m_bitmapFilePattern != value)
   {
      m_bitmapFilePattern = value;
      m_bitmapLoaded = false;
   }
}

template <typename MatrixElementType> void MatrixBitmapEffectBase<MatrixElementType>::Trigger(TableElementData* tableElementData)
{
   if (this->m_matrixLayer != nullptr)
   {
      if (tableElementData->m_value != 0)
      {
         if (!m_active)
         {
            RenderBitmap(tableElementData);
            m_active = true;
         }
         else if (m_retriggerBehaviour == RetriggerBehaviourEnum::Restart)
         {
            RenderBitmap(tableElementData);
         }
      }
      else
      {
         if (m_active)
         {

            MatrixElementType inactiveValue = GetInactiveValue();
            for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
            {
               for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
               {
                  this->m_matrix->SetElement(this->GetLayerNr(), x, y, inactiveValue);
               }
            }
            m_active = false;
         }
      }
   }
}

template <typename MatrixElementType> void MatrixBitmapEffectBase<MatrixElementType>::Init(Table* table)
{
   MatrixEffectBase<MatrixElementType>::Init(table);
   LoadBitmap();
}

template <typename MatrixElementType> void MatrixBitmapEffectBase<MatrixElementType>::Finish()
{
   m_active = false;
   MatrixEffectBase<MatrixElementType>::Finish();
}

template <typename MatrixElementType> void MatrixBitmapEffectBase<MatrixElementType>::LoadBitmap()
{
   if (!m_bitmapFilePattern.empty() && !m_bitmapLoaded)
   {

      std::string filename = m_bitmapFilePattern;

      if (filename.find("{0}") != std::string::npos)
      {
         filename = StringExtensions::Replace(filename, "{0}", std::to_string(m_bitmapFrameNumber));
      }

      m_bitmap = FastBitmap(filename);
      m_bitmapLoaded = m_bitmap.IsValid();

      if (!m_bitmapLoaded)
      {
         Log::Warning(StringExtensions::Build("Could not load bitmap: {0}", filename));
      }
   }
}

template <typename MatrixElementType> void MatrixBitmapEffectBase<MatrixElementType>::RenderBitmap(TableElementData* tableElementData)
{
   if (!m_bitmapLoaded || !m_bitmap.IsValid())
      return;


   int renderWidth = (m_bitmapWidth == -1) ? m_bitmap.GetWidth() : m_bitmapWidth;
   int renderHeight = (m_bitmapHeight == -1) ? m_bitmap.GetHeight() : m_bitmapHeight;


   int maxWidth = this->GetAreaWidth();
   int maxHeight = this->GetAreaHeight();
   renderWidth = std::min(renderWidth, maxWidth);
   renderHeight = std::min(renderHeight, maxHeight);


   for (int y = 0; y < renderHeight; y++)
   {
      for (int x = 0; x < renderWidth; x++)
      {
         int matrixX = this->m_areaLeft + x;
         int matrixY = this->m_areaTop + y;

         if (matrixX <= this->m_areaRight && matrixY <= this->m_areaBottom)
         {
            PixelData pixel = GetScaledPixel(x, y);
            MatrixElementType value = GetPixelValue(pixel, tableElementData->m_value);
            this->m_matrix->SetElement(this->GetLayerNr(), matrixX, matrixY, value);
         }
      }
   }
}

template <typename MatrixElementType> PixelData MatrixBitmapEffectBase<MatrixElementType>::GetScaledPixel(int x, int y) const
{
   if (!m_bitmap.IsValid())
      return PixelData(0, 0, 0, 0);


   int srcX = m_bitmapLeft + ((x * m_bitmap.GetWidth()) / ((m_bitmapWidth == -1) ? m_bitmap.GetWidth() : m_bitmapWidth));
   int srcY = m_bitmapTop + ((y * m_bitmap.GetHeight()) / ((m_bitmapHeight == -1) ? m_bitmap.GetHeight() : m_bitmapHeight));

   return m_bitmap.GetPixel(srcX, srcY);
}

}