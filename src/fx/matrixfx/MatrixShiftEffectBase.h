#pragma once

#include "MatrixEffectBase.h"
#include "MatrixShiftDirectionEnum.h"
#include "../FadeModeEnum.h"
#include "../../general/MathExtensions.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../pinballsupport/AlarmHandler.h"
#include <vector>
#include <map>

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
   float GetShiftSpeed() const { return m_shiftSpeed; }
   void SetShiftSpeed(float value) { m_shiftSpeed = MathExtensions::Limit(value, 1.0f, 10000.0f); }
   float GetShiftAcceleration() const { return m_shiftAcceleration; }
   void SetShiftAcceleration(float value) { m_shiftAcceleration = value; }
   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Init(Table* table) override;
   virtual void Finish() override;

protected:
   virtual MatrixElementType GetEffectValue(int triggerValue) = 0;

private:
   void BuildStep2ElementTable();
   void DoStep();

   static const int RefreshIntervalMs = 30;

   MatrixShiftDirectionEnum m_shiftDirection;
   float m_shiftSpeed;
   float m_shiftAcceleration;

   std::vector<float> m_step2Element;
   std::map<int, int> m_triggerValueBuffer;
   int m_lastTriggerValue;
   int m_lastDiscardedValue;
   int m_currentStep;
   bool m_active;
};


template <typename MatrixElementType>
MatrixShiftEffectBase<MatrixElementType>::MatrixShiftEffectBase()
   : m_shiftDirection(MatrixShiftDirectionEnum::Right)
   , m_shiftSpeed(200.0f)
   , m_shiftAcceleration(0.0f)
   , m_lastTriggerValue(0)
   , m_lastDiscardedValue(0)
   , m_currentStep(0)
   , m_active(false)
{
}

template <typename MatrixElementType> void MatrixShiftEffectBase<MatrixElementType>::BuildStep2ElementTable()
{
   std::vector<float> stepList;

   float numberOfElements
      = (m_shiftDirection == MatrixShiftDirectionEnum::Left || m_shiftDirection == MatrixShiftDirectionEnum::Right) ? (float)this->GetAreaWidth() : (float)this->GetAreaHeight();
   float position = 0.0f;
   float speed = numberOfElements / 100.0f * (m_shiftSpeed / (1000.0f / (float)RefreshIntervalMs));
   float acceleration = numberOfElements / 100.0f * (m_shiftAcceleration / (1000.0f / (float)RefreshIntervalMs));

   while (position <= numberOfElements)
   {
      stepList.push_back(MathExtensions::Limit(position, 0.0f, numberOfElements));
      position += speed;
      speed = MathExtensions::Limit(speed + acceleration, numberOfElements / 100.0f * (1.0f / (1000.0f / (float)RefreshIntervalMs)), 10000.0f);
   }
   stepList.push_back(MathExtensions::Limit(position, 0.0f, numberOfElements));

   m_step2Element = stepList;
}

template <typename MatrixElementType> void MatrixShiftEffectBase<MatrixElementType>::DoStep()
{
   if (!m_active)
   {
      this->m_table->GetPinball()->GetAlarms()->RegisterIntervalAlarm(RefreshIntervalMs, this, [this]() { this->DoStep(); });
      m_active = true;
   }

   int numberOfElements = (m_shiftDirection == MatrixShiftDirectionEnum::Left || m_shiftDirection == MatrixShiftDirectionEnum::Right) ? this->GetAreaWidth() : this->GetAreaHeight();

   float fromElementNr = (float)numberOfElements;
   float toElementNr = 0.0f;
   std::vector<float> value(numberOfElements + 1, 0.0f);

   int lastValue = m_lastDiscardedValue;

   for (auto& kv : m_triggerValueBuffer)
   {
      int stepIndex = m_currentStep - kv.first;
      if (stepIndex >= 0 && stepIndex < (int)m_step2Element.size())
      {
         toElementNr = m_step2Element[stepIndex];

         if ((int)fromElementNr == (int)toElementNr)
         {
            value[(int)fromElementNr] += (fromElementNr - toElementNr) * (float)lastValue;
         }
         else
         {
            if (fromElementNr != (float)(int)fromElementNr)
            {
               value[(int)fromElementNr] += (fromElementNr - (float)(int)fromElementNr) * (float)lastValue;
            }

            int toNr = (int)(toElementNr + 0.999f);
            for (int i = (int)fromElementNr - 1; i >= toNr; i--)
            {
               if (i >= 0 && i < (int)value.size())
                  value[i] = (float)lastValue;
            }
            if (toElementNr != (float)(int)toElementNr)
            {
               value[(int)toElementNr] += ((float)(int)toElementNr + 1.0f - toElementNr) * (float)lastValue;
            }
         }
         fromElementNr = toElementNr;
         lastValue = kv.second;
      }
   }

   // Data output - matches C# lines 157-213
   switch (m_shiftDirection)
   {
   case MatrixShiftDirectionEnum::Right:
      for (int i = 0; i < numberOfElements; i++)
      {
         int v = MathExtensions::Limit((int)value[i], 0, 255);
         if (v > 0 && this->GetFadeMode() == FadeModeEnum::OnOff)
         {
            v = 255;
         }
         MatrixElementType d = GetEffectValue(v);

         for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
         {
            int idx = y * this->m_matrix->GetWidth() + (this->m_areaLeft + i);
            this->m_matrixLayer[idx] = d;
         }
      }
      break;
   case MatrixShiftDirectionEnum::Down:
      for (int i = 0; i < numberOfElements; i++)
      {
         int v = MathExtensions::Limit((int)value[i], 0, 255);
         if (v > 0 && this->GetFadeMode() == FadeModeEnum::OnOff)
         {
            v = 255;
         }
         MatrixElementType d = GetEffectValue(v);

         for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
         {
            int idx = (this->m_areaTop + i) * this->m_matrix->GetWidth() + x;
            this->m_matrixLayer[idx] = d;
         }
      }
      break;
   case MatrixShiftDirectionEnum::Up:
      for (int i = 0; i < numberOfElements; i++)
      {
         int v = MathExtensions::Limit((int)value[i], 0, 255);
         if (v > 0 && this->GetFadeMode() == FadeModeEnum::OnOff)
         {
            v = 255;
         }
         MatrixElementType d = GetEffectValue(v);

         for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
         {
            int idx = (this->m_areaBottom - i) * this->m_matrix->GetWidth() + x;
            this->m_matrixLayer[idx] = d;
         }
      }
      break;
   case MatrixShiftDirectionEnum::Left:
   default:
      for (int i = 0; i < numberOfElements; i++)
      {
         int v = MathExtensions::Limit((int)value[i], 0, 255);
         if (v > 0 && this->GetFadeMode() == FadeModeEnum::OnOff)
         {
            v = 255;
         }
         MatrixElementType d = GetEffectValue(v);

         for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
         {
            int idx = y * this->m_matrix->GetWidth() + (this->m_areaRight - i);
            this->m_matrixLayer[idx] = d;
         }
      }
      break;
   }

   // Clean up old trigger values
   int dropKey = m_currentStep - ((int)m_step2Element.size() - 1);
   auto it = m_triggerValueBuffer.find(dropKey);
   if (it != m_triggerValueBuffer.end())
   {
      m_lastDiscardedValue = it->second;
      m_triggerValueBuffer.erase(it);
   }

   if (!m_triggerValueBuffer.empty() || m_lastDiscardedValue != 0)
   {
      m_currentStep++;
   }
   else
   {
      this->m_table->GetPinball()->GetAlarms()->UnregisterIntervalAlarm(this);
      m_lastDiscardedValue = 0;
      m_currentStep = 0;
      m_active = false;
   }
}

template <typename MatrixElementType> void MatrixShiftEffectBase<MatrixElementType>::Trigger(TableElementData* tableElementData)
{
   if (m_lastTriggerValue != tableElementData->m_value && this->m_matrixLayer != nullptr)
   {
      m_lastTriggerValue = tableElementData->m_value;

      m_triggerValueBuffer[m_currentStep] = m_lastTriggerValue;

      if (!m_active)
      {
         DoStep();
      }
   }
}

template <typename MatrixElementType> void MatrixShiftEffectBase<MatrixElementType>::Init(Table* table)
{
   MatrixEffectBase<MatrixElementType>::Init(table);
   BuildStep2ElementTable();
}

template <typename MatrixElementType> void MatrixShiftEffectBase<MatrixElementType>::Finish()
{
   try
   {
      this->m_table->GetPinball()->GetAlarms()->UnregisterIntervalAlarm(this);
   }
   catch (...)
   {
   }
   MatrixEffectBase<MatrixElementType>::Finish();
}

}