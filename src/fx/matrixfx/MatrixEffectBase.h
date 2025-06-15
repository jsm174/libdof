#pragma once

#include "../EffectBase.h"
#include "IMatrixEffect.h"
#include "../../cab/toys/IMatrixToy.h"
#include "../../cab/toys/ToyList.h"
#include "../../cab/toys/IToy.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../cab/Cabinet.h"
#include "../../general/MathExtensions.h"
#include "../../general/StringExtensions.h"
#include <cmath>

namespace DOF
{

class Table;
class TableElementData;

template <typename MatrixElementType> class MatrixEffectBase : public EffectBase, public virtual IMatrixEffect
{
public:
   MatrixEffectBase();
   virtual ~MatrixEffectBase() = default;

   virtual FadeModeEnum GetFadeMode() const override { return m_fadeMode; }
   virtual void SetFadeMode(FadeModeEnum value) override { m_fadeMode = value; }
   virtual float GetHeight() const override { return m_height; }
   virtual void SetHeight(float value) override { m_height = MathExtensions::Limit(value, 0.0f, 100.0f); }
   virtual int GetLayerNr() const override { return m_layerNr; }
   virtual void SetLayerNr(int value) override { m_layerNr = value; }
   virtual float GetLeft() const override { return m_left; }
   virtual void SetLeft(float value) override { m_left = MathExtensions::Limit(value, 0.0f, 100.0f); }
   virtual float GetTop() const override { return m_top; }
   virtual void SetTop(float value) override { m_top = MathExtensions::Limit(value, 0.0f, 100.0f); }
   virtual const std::string& GetToyName() const override { return m_toyName; }
   virtual void SetToyName(const std::string& value) override;
   virtual float GetWidth() const override { return m_width; }
   virtual void SetWidth(float value) override { m_width = MathExtensions::Limit(value, 0.0f, 100.0f); }
   virtual void Init(Table* table) override;
   virtual void Finish() override;

protected:
   IMatrixToy<MatrixElementType>* m_matrix;
   MatrixElementType* m_matrixLayer;
   Table* m_table;
   int m_areaLeft;
   int m_areaTop;
   int m_areaRight;
   int m_areaBottom;

   int GetAreaWidth() const { return (m_areaRight - m_areaLeft) + 1; }
   int GetAreaHeight() const { return (m_areaBottom - m_areaTop) + 1; }

private:
   std::string m_toyName;
   float m_width;
   float m_height;
   float m_left;
   float m_top;
   int m_layerNr;
   FadeModeEnum m_fadeMode;
};

template <typename MatrixElementType>
MatrixEffectBase<MatrixElementType>::MatrixEffectBase()
   : m_matrix(nullptr)
   , m_matrixLayer(nullptr)
   , m_table(nullptr)
   , m_areaLeft(0)
   , m_areaTop(0)
   , m_areaRight(0)
   , m_areaBottom(0)
   , m_width(100.0f)
   , m_height(100.0f)
   , m_left(0.0f)
   , m_top(0.0f)
   , m_layerNr(0)
   , m_fadeMode(FadeModeEnum::Fade)
{
}

template <typename MatrixElementType> void MatrixEffectBase<MatrixElementType>::SetToyName(const std::string& value)
{
   if (m_toyName != value)
   {
      m_toyName = value;
      m_matrix = nullptr;
      m_matrixLayer = nullptr;
   }
}

template <typename MatrixElementType> void MatrixEffectBase<MatrixElementType>::Init(Table* table)
{
   if (!StringExtensions::IsNullOrWhiteSpace(m_toyName) && table->GetPinball()->GetCabinet()->GetToys()->Contains(m_toyName))
   {
      IToy* toy = table->GetPinball()->GetCabinet()->GetToys()->FindByName(m_toyName);
      IMatrixToy<MatrixElementType>* matrixToy = dynamic_cast<IMatrixToy<MatrixElementType>*>(toy);
      if (matrixToy != nullptr)
      {
         m_matrix = matrixToy;
         m_matrixLayer = m_matrix->GetLayer(m_layerNr);

         m_areaLeft = MathExtensions::Limit((int)std::floor((float)m_matrix->GetWidth() / 100.0f * m_left), 0, m_matrix->GetWidth() - 1);
         m_areaTop = MathExtensions::Limit((int)std::floor((float)m_matrix->GetHeight() / 100.0f * m_top), 0, m_matrix->GetHeight() - 1);
         m_areaRight = MathExtensions::Limit((int)std::floor((float)m_matrix->GetWidth() / 100.0f * MathExtensions::Limit(m_left + m_width, 0.0f, 100.0f)), 0, m_matrix->GetWidth() - 1);
         m_areaBottom = MathExtensions::Limit((int)std::floor((float)m_matrix->GetHeight() / 100.0f * MathExtensions::Limit(m_top + m_height, 0.0f, 100.0f)), 0, m_matrix->GetHeight() - 1);

         if (m_areaLeft > m_areaRight)
         {
            int tmp = m_areaRight;
            m_areaRight = m_areaLeft;
            m_areaLeft = tmp;
         }
         if (m_areaTop > m_areaBottom)
         {
            int tmp = m_areaBottom;
            m_areaBottom = m_areaTop;
            m_areaTop = tmp;
         }
      }
   }

   m_table = table;
}

template <typename MatrixElementType> void MatrixEffectBase<MatrixElementType>::Finish()
{
   m_matrixLayer = nullptr;
   m_matrix = nullptr;
   m_table = nullptr;
   EffectBase::Finish();
}

}