#pragma once

#include "MatrixEffectBase.h"
#include "../../table/TableElementData.h"
#include "../../general/MathExtensions.h"

namespace DOF
{

template <typename MatrixElementType> class MatrixValueEffectBase : public MatrixEffectBase<MatrixElementType>
{
public:
   MatrixValueEffectBase() = default;
   virtual ~MatrixValueEffectBase() = default;

   virtual void Trigger(TableElementData* tableElementData) override;

protected:
   virtual MatrixElementType GetEffectValue(int triggerValue) = 0;
};

template <typename MatrixElementType> void MatrixValueEffectBase<MatrixElementType>::Trigger(TableElementData* tableElementData)
{
   if (this->m_matrixLayer != nullptr)
   {
      int v = MathExtensions::Limit(tableElementData->m_value, 0, 255);
      if (v > 0 && this->GetFadeMode() == FadeModeEnum::OnOff)
         v = 255;

      MatrixElementType d = GetEffectValue(v);

      for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
      {
         for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
            this->m_matrix->SetElement(this->GetLayerNr(), x, y, d);
      }
   }
}

}