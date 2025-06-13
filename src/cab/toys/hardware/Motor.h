#pragma once

#include "../layer/AnalogAlphaToy.h"

namespace DOF
{

class Cabinet;
class AlarmHandler;

class Motor : public AnalogAlphaToy
{
public:
   Motor();
   virtual ~Motor();

   virtual void Init(Cabinet* cabinet) override;
   virtual void Finish() override;
   virtual void UpdateOutputs() override;
   int GetMaxRunTimeMs() const { return m_maxRunTimeMs; }
   void SetMaxRunTimeMs(int value);
   int GetKickstartPower() const { return m_kickstartPower; }
   void SetKickstartPower(int value);
   int GetKickstartDurationMs() const { return m_kickstartDurationMs; }
   void SetKickstartDurationMs(int value);
   int GetMinPower() const { return m_minPower; }
   void SetMinPower(int value);
   int GetMaxPower() const { return m_maxPower; }
   void SetMaxPower(int value);

private:
   int m_maxRunTimeMs;
   int m_kickstartPower;
   int m_kickstartDurationMs;
   int m_minPower;
   int m_maxPower;
   int m_currentMotorPower;
   int m_targetMotorPower;
   bool m_kickstartActive;
   bool m_turnedOffAfterMaxRunTime;
   AlarmHandler* m_alarmHandler;
   void MaxRunTimeMotorStop();
   void KickStartEnd();
};

}