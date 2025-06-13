#include "Motor.h"
#include "../../Cabinet.h"
#include "../../../pinballsupport/AlarmHandler.h"
#include "../../out/IOutput.h"
#include "../../../Log.h"
#include "../../../general/MathExtensions.h"
#include <algorithm>
#include <functional>

namespace DOF
{

Motor::Motor()
   : m_maxRunTimeMs(300000)
   , m_kickstartPower(255)
   , m_kickstartDurationMs(100)
   , m_minPower(1)
   , m_maxPower(255)
   , m_currentMotorPower(0)
   , m_targetMotorPower(0)
   , m_kickstartActive(false)
   , m_turnedOffAfterMaxRunTime(false)
   , m_alarmHandler(nullptr)
{
}

Motor::~Motor() { }

void Motor::Init(Cabinet* cabinet)
{
   AnalogAlphaToy::Init(cabinet);

   if (cabinet != nullptr)
   {
      m_alarmHandler = cabinet->GetAlarms();
   }
}

void Motor::Finish()
{
   if (m_alarmHandler != nullptr)
   {


      m_alarmHandler = nullptr;
   }

   AnalogAlphaToy::Finish();
}

void Motor::UpdateOutputs()
{
   IOutput* output = GetOutput();
   if (output == nullptr)
      return;


   int P = GetCurrentValue().GetValue();
   P = MathExtensions::Limit(P, 0, 255);


   if (P != 0)
   {

      int powerRange = (m_maxPower >= m_minPower) ? (m_maxPower - m_minPower) : (m_minPower - m_maxPower);
      P = static_cast<int>((static_cast<double>(powerRange) / 255.0 * P) + m_minPower);
      P = MathExtensions::Limit(P, m_minPower, m_maxPower);
   }

   if (P == 0)
   {
      m_turnedOffAfterMaxRunTime = false;
   }

   if (!m_turnedOffAfterMaxRunTime)
   {
      if (m_currentMotorPower == 0)
      {

         if (P > 0)
         {

            if (m_kickstartDurationMs > 0 && m_kickstartPower > 0 && P <= m_kickstartPower)
            {

               m_targetMotorPower = P;

               if (!m_kickstartActive)
               {
                  m_currentMotorPower = m_kickstartPower;
                  output->SetOutput(static_cast<uint8_t>(m_currentMotorPower));
                  m_kickstartActive = true;

                  if (m_alarmHandler != nullptr)
                  {
                  }
               }
            }
            else
            {

               m_currentMotorPower = P;
               m_targetMotorPower = P;
               output->SetOutput(static_cast<uint8_t>(P));
               m_kickstartActive = false;
            }

            if (m_maxRunTimeMs > 0 && m_alarmHandler != nullptr)
            {
            }
         }
      }
      else if (m_kickstartActive)
      {

         if (P > 0)
         {

            m_targetMotorPower = P;
         }
         else
         {

            if (m_alarmHandler != nullptr)
            {
            }
            m_targetMotorPower = 0;
            m_currentMotorPower = 0;
            output->SetOutput(0);
         }
      }
      else
      {

         if (P == 0)
         {

            if (m_alarmHandler != nullptr)
            {
            }
            m_targetMotorPower = 0;
            m_currentMotorPower = 0;
            output->SetOutput(0);
         }
         else if (P != m_currentMotorPower)
         {

            m_currentMotorPower = P;
            m_targetMotorPower = P;
            output->SetOutput(static_cast<uint8_t>(P));
         }
      }
   }
}

void Motor::MaxRunTimeMotorStop()
{
   if (m_alarmHandler != nullptr)
   {
   }

   m_kickstartActive = false;
   m_currentMotorPower = 0;
   m_targetMotorPower = 0;
   m_turnedOffAfterMaxRunTime = true;

   IOutput* output = GetOutput();
   if (output != nullptr)
   {
      output->SetOutput(0);
   }
}

void Motor::KickStartEnd()
{
   m_kickstartActive = false;
   m_currentMotorPower = m_targetMotorPower;

   IOutput* output = GetOutput();
   if (output != nullptr)
   {
      output->SetOutput(static_cast<uint8_t>(m_currentMotorPower));
   }
}

void Motor::SetMaxRunTimeMs(int value) { m_maxRunTimeMs = MathExtensions::Limit(value, 0, INT_MAX); }

void Motor::SetKickstartPower(int value) { m_kickstartPower = MathExtensions::Limit(value, 0, 255); }

void Motor::SetKickstartDurationMs(int value) { m_kickstartDurationMs = MathExtensions::Limit(value, 0, 5000); }

void Motor::SetMinPower(int value) { m_minPower = MathExtensions::Limit(value, 0, 255); }

void Motor::SetMaxPower(int value) { m_maxPower = MathExtensions::Limit(value, 0, 255); }


}