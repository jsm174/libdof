#pragma once

#include "MatrixEffectBase.h"
#include "../RetriggerBehaviourEnum.h"
#include "../../general/MathExtensions.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../Pinball.h"
#include "../../pinballsupport/AlarmHandler.h"
#include "../../pinballsupport/Action.h"
#include <cmath>
#include <chrono>

namespace DOF
{

class TableElementData;

template <typename MatrixElementType> class MatrixPlasmaEffectBase : public MatrixEffectBase<MatrixElementType>
{
public:
   MatrixPlasmaEffectBase();
   virtual ~MatrixPlasmaEffectBase() = default;

   int GetPlasmaSpeed() const { return m_plasmaSpeed; }
   void SetPlasmaSpeed(int value) { m_plasmaSpeed = MathExtensions::Limit(value, 1, 100); }
   int GetPlasmaDensity() const { return m_plasmaDensity; }
   void SetPlasmaDensity(int value) { m_plasmaDensity = MathExtensions::Limit(value, 1, 100); }
   RetriggerBehaviourEnum GetRetriggerBehaviour() const { return m_retriggerBehaviour; }
   void SetRetriggerBehaviour(RetriggerBehaviourEnum value) { m_retriggerBehaviour = value; }
   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Finish() override;

protected:
   virtual MatrixElementType GetInactiveValue() = 0;
   virtual MatrixElementType GetPlasmaValue(int triggerValue, double intensity) = 0;

private:
   void StartPlasma(TableElementData* tableElementData);
   void StopPlasma();
   void PlasmaStep();
   void UpdateMatrix();
   double CalculatePlasmaIntensity(int x, int y, double time);

   int m_plasmaSpeed;
   int m_plasmaDensity;
   RetriggerBehaviourEnum m_retriggerBehaviour;
   bool m_active;
   TableElementData* m_plasmaTableElementData;
   std::chrono::steady_clock::time_point m_startTime;
};


template <typename MatrixElementType>
MatrixPlasmaEffectBase<MatrixElementType>::MatrixPlasmaEffectBase()
   : m_plasmaSpeed(50)
   , m_plasmaDensity(50)
   , m_retriggerBehaviour(RetriggerBehaviourEnum::Restart)
   , m_active(false)
   , m_plasmaTableElementData(nullptr)
{
}

template <typename MatrixElementType> void MatrixPlasmaEffectBase<MatrixElementType>::Trigger(TableElementData* tableElementData)
{
   if (this->m_matrixLayer != nullptr)
   {
      if (tableElementData->m_value != 0)
      {
         if (!m_active)
         {
            StartPlasma(tableElementData);
         }
         else if (m_retriggerBehaviour == RetriggerBehaviourEnum::Restart)
         {
            StartPlasma(tableElementData);
         }
      }
      else
      {
         if (m_active)
         {
            StopPlasma();
         }
      }
   }
}

template <typename MatrixElementType> void MatrixPlasmaEffectBase<MatrixElementType>::StartPlasma(TableElementData* tableElementData)
{
   m_plasmaTableElementData = tableElementData;
   m_active = true;
   m_startTime = std::chrono::steady_clock::now();

   UpdateMatrix();
   PlasmaStep();
}

template <typename MatrixElementType> void MatrixPlasmaEffectBase<MatrixElementType>::StopPlasma()
{
   m_active = false;
   m_plasmaTableElementData = nullptr;


   MatrixElementType inactiveValue = GetInactiveValue();
   for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
   {
      for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
      {
         this->m_matrix->SetElement(this->GetLayerNr(), x, y, inactiveValue);
      }
   }
}

template <typename MatrixElementType> void MatrixPlasmaEffectBase<MatrixElementType>::PlasmaStep()
{
   if (!m_active || m_plasmaTableElementData == nullptr)
      return;

   UpdateMatrix();


   int stepDelayMs = MathExtensions::Limit(110 - m_plasmaSpeed, 10, 100);
   this->m_table->GetPinball()->GetAlarms()->RegisterAlarm(stepDelayMs, Action(this, &MatrixPlasmaEffectBase<MatrixElementType>::PlasmaStep), false);
}

template <typename MatrixElementType> void MatrixPlasmaEffectBase<MatrixElementType>::UpdateMatrix()
{
   if (!m_active || m_plasmaTableElementData == nullptr)
      return;

   auto now = std::chrono::steady_clock::now();
   auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
   double time = elapsed / 1000.0;

   for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
   {
      for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
      {
         double intensity = CalculatePlasmaIntensity(x, y, time);
         MatrixElementType value = GetPlasmaValue(m_plasmaTableElementData->m_value, intensity);
         this->m_matrix->SetElement(this->GetLayerNr(), x, y, value);
      }
   }
}

template <typename MatrixElementType> double MatrixPlasmaEffectBase<MatrixElementType>::CalculatePlasmaIntensity(int x, int y, double time)
{

   double normX = (double)(x - this->m_areaLeft) / (double)this->GetAreaWidth();
   double normY = (double)(y - this->m_areaTop) / (double)this->GetAreaHeight();


   double densityFactor = m_plasmaDensity / 50.0;


   double plasma1 = std::sin(normX * 10.0 * densityFactor + time);
   double plasma2 = std::sin(normY * 10.0 * densityFactor + time * 1.3);
   double plasma3 = std::sin((normX + normY) * 8.0 * densityFactor + time * 0.7);
   double plasma4 = std::sin(std::sqrt(normX * normX + normY * normY) * 12.0 * densityFactor + time * 1.1);


   double combined = (plasma1 + plasma2 + plasma3 + plasma4) / 4.0;
   return (combined + 1.0) / 2.0;
}

template <typename MatrixElementType> void MatrixPlasmaEffectBase<MatrixElementType>::Finish()
{
   try
   {
   }
   catch (...)
   {
   }
   m_active = false;
   m_plasmaTableElementData = nullptr;
   MatrixEffectBase<MatrixElementType>::Finish();
}

}