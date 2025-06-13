#pragma once

#include "../ToyBase.h"
#include "../../out/IOutput.h"
#include <vector>
#include <string>

namespace DOF
{

class LedWizEquivalentOutput
{
public:
   LedWizEquivalentOutput();
   LedWizEquivalentOutput(int outputNumber, const std::string& outputName);
   ~LedWizEquivalentOutput();

   const std::string& GetOutputName() const { return m_outputName; }
   void SetOutputName(const std::string& name) { m_outputName = name; }
   int GetLedWizEquivalentOutputNumber() const { return m_ledWizEquivalentOutputNumber; }
   void SetLedWizEquivalentOutputNumber(int number) { m_ledWizEquivalentOutputNumber = number; }
   IOutput* GetOutput() const { return m_output; }
   void SetOutput(IOutput* output) { m_output = output; }

private:
   std::string m_outputName;
   int m_ledWizEquivalentOutputNumber;
   IOutput* m_output;
};

class LedWizEquivalentOutputList : public std::vector<LedWizEquivalentOutput*>
{
public:
   LedWizEquivalentOutputList();
   ~LedWizEquivalentOutputList();

   LedWizEquivalentOutput* FindByNumber(int number) const;
   LedWizEquivalentOutput* FindByName(const std::string& name) const;
   void AddOutput(LedWizEquivalentOutput* output);
   LedWizEquivalentOutput* GetOrCreateOutput(int number);
};

class LedWizEquivalent : public ToyBase
{
public:
   LedWizEquivalent();
   LedWizEquivalent(int ledWizNumber);
   virtual ~LedWizEquivalent();

   virtual void Init(Cabinet* cabinet) override;
   virtual void Reset() override;
   virtual void Finish() override;

   LedWizEquivalentOutputList& GetOutputs() { return m_outputs; }
   const LedWizEquivalentOutputList& GetOutputs() const { return m_outputs; }
   int GetLedWizNumber() const { return m_ledWizNumber; }
   void SetLedWizNumber(int number) { m_ledWizNumber = number; }
   IOutput* GetOutput(int outputNumber);
   void SetupOutputs(int numberOfOutputs = 32);

private:
   LedWizEquivalentOutputList m_outputs;
   int m_ledWizNumber;

   void ResolveOutputs(Cabinet* cabinet);
};

}