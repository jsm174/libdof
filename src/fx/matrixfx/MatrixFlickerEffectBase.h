#pragma once

#include "MatrixEffectBase.h"
#include "../RetriggerBehaviourEnum.h"
#include "../../general/MathExtensions.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../pinballsupport/AlarmHandler.h"
#include <random>
#include <chrono>

namespace DOF
{

class TableElementData;

template <typename MatrixElementType> class MatrixFlickerEffectBase : public MatrixEffectBase<MatrixElementType>
{
public:
   MatrixFlickerEffectBase();
   virtual ~MatrixFlickerEffectBase() = default;

   int GetDensity() const { return m_density; }
   void SetDensity(int value) { m_density = MathExtensions::Limit(value, 1, 100); }
   int GetMinFlickerDurationMs() const { return m_minFlickerDurationMs; }
   void SetMinFlickerDurationMs(int value) { m_minFlickerDurationMs = MathExtensions::Limit(value, 1, 10000); }
   int GetMaxFlickerDurationMs() const { return m_maxFlickerDurationMs; }
   void SetMaxFlickerDurationMs(int value) { m_maxFlickerDurationMs = MathExtensions::Limit(value, 1, 10000); }
   int GetFadeDurationMs() const { return m_fadeDurationMs; }
   void SetFadeDurationMs(int value) { m_fadeDurationMs = MathExtensions::Limit(value, 0, 10000); }
   RetriggerBehaviourEnum GetRetriggerBehaviour() const { return m_retriggerBehaviour; }
   void SetRetriggerBehaviour(RetriggerBehaviourEnum value) { m_retriggerBehaviour = value; }
   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Finish() override;

protected:
   virtual MatrixElementType GetInactiveValue() = 0;
   virtual MatrixElementType GetActiveValue(int triggerValue) = 0;

private:
   void StartFlicker(TableElementData* tableElementData);
   void StopFlicker();
   void FlickerStep();

   int m_density;
   int m_minFlickerDurationMs;
   int m_maxFlickerDurationMs;
   int m_fadeDurationMs;
   RetriggerBehaviourEnum m_retriggerBehaviour;
   bool m_active;
   TableElementData* m_flickerTableElementData;
   std::mt19937 m_randomGenerator;
};

template <typename MatrixElementType>
MatrixFlickerEffectBase<MatrixElementType>::MatrixFlickerEffectBase()
   : m_density(50)
   , m_minFlickerDurationMs(100)
   , m_maxFlickerDurationMs(300)
   , m_fadeDurationMs(0)
   , m_retriggerBehaviour(RetriggerBehaviourEnum::Restart)
   , m_active(false)
   , m_flickerTableElementData(nullptr)
   , m_randomGenerator(std::random_device {}())
{
}

template <typename MatrixElementType> void MatrixFlickerEffectBase<MatrixElementType>::Trigger(TableElementData* tableElementData)
{
   if (this->m_matrixLayer != nullptr)
   {
      if (tableElementData->m_value != 0)
      {
         if (!m_active)
            StartFlicker(tableElementData);
         else if (m_retriggerBehaviour == RetriggerBehaviourEnum::Restart)
            StartFlicker(tableElementData);
      }
      else
      {
         if (m_active)
            StopFlicker();
      }
   }
}

template <typename MatrixElementType> void MatrixFlickerEffectBase<MatrixElementType>::StartFlicker(TableElementData* tableElementData)
{
   m_flickerTableElementData = tableElementData;
   m_active = true;
   FlickerStep();
}

template <typename MatrixElementType> void MatrixFlickerEffectBase<MatrixElementType>::StopFlicker()
{
   m_active = false;
   m_flickerTableElementData = nullptr;

   MatrixElementType inactiveValue = GetInactiveValue();
   for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
   {
      for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
         this->m_matrix->SetElement(this->GetLayerNr(), x, y, inactiveValue);
   }
}

template <typename MatrixElementType> void MatrixFlickerEffectBase<MatrixElementType>::FlickerStep()
{
   if (!m_active || m_flickerTableElementData == nullptr)
      return;

   std::uniform_int_distribution<int> densityDist(1, 100);
   std::uniform_int_distribution<int> durationDist(m_minFlickerDurationMs, m_maxFlickerDurationMs);

   MatrixElementType activeValue = GetActiveValue(m_flickerTableElementData->m_value);
   MatrixElementType inactiveValue = GetInactiveValue();

   for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
   {
      for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
      {
         bool shouldFlicker = densityDist(m_randomGenerator) <= m_density;
         this->m_matrix->SetElement(this->GetLayerNr(), x, y, shouldFlicker ? activeValue : inactiveValue);
      }
   }

   int nextStepMs = durationDist(m_randomGenerator);
   this->m_table->GetPinball()->GetAlarms()->RegisterAlarm(nextStepMs, [this]() { this->FlickerStep(); }, false);
}

template <typename MatrixElementType> void MatrixFlickerEffectBase<MatrixElementType>::Finish()
{
   try
   {
   }
   catch (...)
   {
   }
   m_active = false;
   m_flickerTableElementData = nullptr;
   MatrixEffectBase<MatrixElementType>::Finish();
}

}