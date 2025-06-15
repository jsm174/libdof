#pragma once

#include "MatrixEffectBase.h"
#include "MatrixShiftDirectionEnum.h"
#include "../RetriggerBehaviourEnum.h"
#include "../../general/MathExtensions.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../pinballsupport/AlarmHandler.h"
#include <chrono>

namespace DOF
{

class TableElementData;

template <typename MatrixElementType> class MatrixShiftEffectBase : public MatrixEffectBase<MatrixElementType>
{
public:
   MatrixShiftEffectBase();
   virtual ~MatrixShiftEffectBase() = default;

   MatrixShiftDirectionEnum GetShiftDirection() const { return m_shiftDirection; }
   void SetShiftDirection(MatrixShiftDirectionEnum value) { m_shiftDirection = value; }
   int GetShiftSpeed() const { return m_shiftSpeed; }
   void SetShiftSpeed(int value) { m_shiftSpeed = MathExtensions::Limit(value, 1, 1000); }
   int GetShiftAcceleration() const { return m_shiftAcceleration; }
   void SetShiftAcceleration(int value) { m_shiftAcceleration = MathExtensions::Limit(value, -1000, 1000); }
   RetriggerBehaviourEnum GetRetriggerBehaviour() const { return m_retriggerBehaviour; }
   void SetRetriggerBehaviour(RetriggerBehaviourEnum value) { m_retriggerBehaviour = value; }
   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Finish() override;

protected:
   virtual MatrixElementType GetInactiveValue() = 0;
   virtual MatrixElementType GetActiveValue(int triggerValue) = 0;

private:
   void StartShift(TableElementData* tableElementData);
   void StopShift();
   void ShiftStep();
   void UpdateMatrix();

   MatrixShiftDirectionEnum m_shiftDirection;
   int m_shiftSpeed;
   int m_shiftAcceleration;
   RetriggerBehaviourEnum m_retriggerBehaviour;
   bool m_active;
   TableElementData* m_shiftTableElementData;
   int m_currentSpeed;
   int m_shiftPosition;
   std::chrono::steady_clock::time_point m_lastShiftTime;
};


template <typename MatrixElementType>
MatrixShiftEffectBase<MatrixElementType>::MatrixShiftEffectBase()
   : m_shiftDirection(MatrixShiftDirectionEnum::Left)
   , m_shiftSpeed(100)
   , m_shiftAcceleration(0)
   , m_retriggerBehaviour(RetriggerBehaviourEnum::Restart)
   , m_active(false)
   , m_shiftTableElementData(nullptr)
   , m_currentSpeed(0)
   , m_shiftPosition(0)
{
}

template <typename MatrixElementType> void MatrixShiftEffectBase<MatrixElementType>::Trigger(TableElementData* tableElementData)
{
   if (this->m_matrixLayer != nullptr)
   {
      if (tableElementData->m_value != 0)
      {
         if (!m_active)
         {
            StartShift(tableElementData);
         }
         else if (m_retriggerBehaviour == RetriggerBehaviourEnum::Restart)
         {
            StartShift(tableElementData);
         }
      }
      else
      {
         if (m_active)
         {
            StopShift();
         }
      }
   }
}

template <typename MatrixElementType> void MatrixShiftEffectBase<MatrixElementType>::StartShift(TableElementData* tableElementData)
{
   m_shiftTableElementData = tableElementData;
   m_active = true;
   m_currentSpeed = m_shiftSpeed;
   m_shiftPosition = 0;
   m_lastShiftTime = std::chrono::steady_clock::now();

   UpdateMatrix();
   ShiftStep();
}

template <typename MatrixElementType> void MatrixShiftEffectBase<MatrixElementType>::StopShift()
{
   m_active = false;
   m_shiftTableElementData = nullptr;


   MatrixElementType inactiveValue = GetInactiveValue();
   for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
   {
      for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
      {
         this->m_matrix->SetElement(this->GetLayerNr(), x, y, inactiveValue);
      }
   }
}

template <typename MatrixElementType> void MatrixShiftEffectBase<MatrixElementType>::ShiftStep()
{
   if (!m_active || m_shiftTableElementData == nullptr)
      return;

   auto now = std::chrono::steady_clock::now();
   auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastShiftTime).count();

   if (elapsed >= 50)
   {

      if (m_shiftAcceleration != 0)
      {
         m_currentSpeed += m_shiftAcceleration;
         m_currentSpeed = MathExtensions::Limit(m_currentSpeed, 1, 1000);
      }


      m_shiftPosition++;
      m_lastShiftTime = now;

      UpdateMatrix();
   }


   this->m_table->GetPinball()->GetAlarms()->RegisterAlarm(20, [this]() { this->ShiftStep(); }, false);
}

template <typename MatrixElementType> void MatrixShiftEffectBase<MatrixElementType>::UpdateMatrix()
{
   if (!m_active || m_shiftTableElementData == nullptr)
      return;

   MatrixElementType activeValue = GetActiveValue(m_shiftTableElementData->m_value);
   MatrixElementType inactiveValue = GetInactiveValue();


   for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
   {
      for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
      {
         this->m_matrix->SetElement(this->GetLayerNr(), x, y, inactiveValue);
      }
   }


   int areaWidth = this->GetAreaWidth();
   int areaHeight = this->GetAreaHeight();

   switch (m_shiftDirection)
   {
   case MatrixShiftDirectionEnum::Left:
   {
      int xPos = this->m_areaLeft + (m_shiftPosition % (areaWidth * 2));
      if (xPos < this->m_areaRight)
      {
         for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
         {
            this->m_matrix->SetElement(this->GetLayerNr(), xPos, y, activeValue);
         }
      }
   }
   break;

   case MatrixShiftDirectionEnum::Right:
   {
      int xPos = this->m_areaRight - (m_shiftPosition % (areaWidth * 2));
      if (xPos >= this->m_areaLeft)
      {
         for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
         {
            this->m_matrix->SetElement(this->GetLayerNr(), xPos, y, activeValue);
         }
      }
   }
   break;

   case MatrixShiftDirectionEnum::Up:
   {
      int yPos = this->m_areaTop + (m_shiftPosition % (areaHeight * 2));
      if (yPos <= this->m_areaBottom)
      {
         for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
         {
            this->m_matrix->SetElement(this->GetLayerNr(), x, yPos, activeValue);
         }
      }
   }
   break;

   case MatrixShiftDirectionEnum::Down:
   {
      int yPos = this->m_areaBottom - (m_shiftPosition % (areaHeight * 2));
      if (yPos >= this->m_areaTop)
      {
         for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
         {
            this->m_matrix->SetElement(this->GetLayerNr(), x, yPos, activeValue);
         }
      }
   }
   break;
   }
}

template <typename MatrixElementType> void MatrixShiftEffectBase<MatrixElementType>::Finish()
{
   try
   {
   }
   catch (...)
   {
   }
   m_active = false;
   m_shiftTableElementData = nullptr;
   MatrixEffectBase<MatrixElementType>::Finish();
}

}