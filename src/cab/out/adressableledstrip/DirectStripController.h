#pragma once

#include "DOF/DOF.h"
#include "../OutputControllerBase.h"
#include "../ISupportsSetValues.h"
#include "DirectStripControllerApi.h"
#include <vector>
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>

namespace DOF
{

class DirectStripControllerApi;

class DirectStripController : public OutputControllerBase, public ISupportsSetValues
{
public:
   DirectStripController();
   virtual ~DirectStripController();

   virtual void SetValues(int firstOutput, const uint8_t* values, int valueCount) override;

   int GetControllerNumber() const { return m_controllerNumber; }
   void SetControllerNumber(int value) { m_controllerNumber = value; }

   int GetNumberOfLeds() const { return m_numberOfLeds; }
   void SetNumberOfLeds(int value);

   bool GetPackData() const { return m_packData; }
   void SetPackData(bool value) { m_packData = value; }

   virtual void Init(Cabinet* cabinet) override;
   virtual void Finish() override;
   virtual void Update() override;

   virtual std::string GetXmlElementName() const override { return "DirectStripController"; }

protected:
   virtual void OnOutputValueChanged(IOutput* output) override;

private:
   void AddOutputs();
   void InitUpdaterThread();
   void FinishUpdaterThread();
   void UpdaterThreadSignal();
   bool IsUpdaterThreadActive() const;
   void UpdaterThreadDoIt();

   static const std::vector<int> s_colNrLookup;

   std::vector<uint8_t> m_ledData;
   std::vector<uint8_t> m_outputLedData;

   int m_controllerNumber;
   int m_numberOfLeds;
   bool m_packData;

   mutable std::mutex m_updateLocker;
   bool m_updateRequired;

   std::thread* m_updaterThread;
   mutable std::mutex m_updaterThreadLocker;
   std::condition_variable m_updateCondition;
   mutable std::mutex m_conditionMutex;
   bool m_keepUpdaterThreadAlive;

   DirectStripControllerApi* m_controller;
};

}