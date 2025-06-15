#include "LedWizEquivalent.h"
#include "../../Cabinet.h"
#include "../../out/OutputControllerList.h"
#include "../../out/IOutputController.h"
#include "../../out/OutputControllerBase.h"
#include "../../out/OutputList.h"
#include "../../out/Output.h"
#include "../../../Log.h"
#include "../../../general/StringExtensions.h"

#include <algorithm>

namespace DOF
{


LedWizEquivalentOutput::LedWizEquivalentOutput()
   : m_ledWizEquivalentOutputNumber(0)
   , m_output(nullptr)
{
}

LedWizEquivalentOutput::LedWizEquivalentOutput(int outputNumber, const std::string& outputName)
   : m_outputName(outputName)
   , m_ledWizEquivalentOutputNumber(outputNumber)
   , m_output(nullptr)
{
}

LedWizEquivalentOutput::~LedWizEquivalentOutput() { }


LedWizEquivalentOutputList::LedWizEquivalentOutputList() { }

LedWizEquivalentOutputList::~LedWizEquivalentOutputList()
{

   for (LedWizEquivalentOutput* output : *this)
   {
      delete output;
   }
   clear();
}

LedWizEquivalentOutput* LedWizEquivalentOutputList::FindByNumber(int number) const
{
   for (LedWizEquivalentOutput* output : *this)
   {
      if (output != nullptr && output->GetLedWizEquivalentOutputNumber() == number)
      {
         return output;
      }
   }
   return nullptr;
}

LedWizEquivalentOutput* LedWizEquivalentOutputList::FindByName(const std::string& name) const
{
   for (LedWizEquivalentOutput* output : *this)
   {
      if (output != nullptr && output->GetOutputName() == name)
      {
         return output;
      }
   }
   return nullptr;
}

void LedWizEquivalentOutputList::AddOutput(LedWizEquivalentOutput* output)
{
   if (output == nullptr)
      return;


   if (FindByNumber(output->GetLedWizEquivalentOutputNumber()) != nullptr)
   {
      Log::Warning(StringExtensions::Build("LedWizEquivalentOutput with number {0} already exists", std::to_string(output->GetLedWizEquivalentOutputNumber())));
      return;
   }

   push_back(output);
}

LedWizEquivalentOutput* LedWizEquivalentOutputList::GetOrCreateOutput(int number)
{
   LedWizEquivalentOutput* output = FindByNumber(number);
   if (output == nullptr)
   {
      output = new LedWizEquivalentOutput(number, "");
      AddOutput(output);
   }
   return output;
}


LedWizEquivalent::LedWizEquivalent()
   : m_ledWizNumber(-1)
{
}

LedWizEquivalent::LedWizEquivalent(int ledWizNumber)
   : m_ledWizNumber(ledWizNumber)
{
}

LedWizEquivalent::~LedWizEquivalent() { }

void LedWizEquivalent::Init(Cabinet* cabinet)
{
   if (cabinet == nullptr)
   {
      Log::Warning("LedWizEquivalent::Init - Cabinet is null");
      return;
   }

   Log::Write(StringExtensions::Build("Initializing LedWizEquivalent: {0} (LedWiz {1})", GetName(), std::to_string(m_ledWizNumber)));

   ResolveOutputs(cabinet);

   Log::Write(StringExtensions::Build("LedWizEquivalent {0} initialized with {1} outputs", std::to_string(m_ledWizNumber), std::to_string(static_cast<int>(m_outputs.size()))));
}

void LedWizEquivalent::Reset()
{
   for (LedWizEquivalentOutput* output : m_outputs)
   {
      if (output != nullptr && output->GetOutput() != nullptr)
         output->GetOutput()->SetOutput(0);
   }
}

void LedWizEquivalent::Finish()
{
   Log::Write(StringExtensions::Build("Finishing LedWizEquivalent: {0} (LedWiz {1})", GetName(), std::to_string(m_ledWizNumber)));
   Reset();
}


IOutput* LedWizEquivalent::GetOutput(int outputNumber)
{
   LedWizEquivalentOutput* ledWizOutput = m_outputs.FindByNumber(outputNumber);
   if (ledWizOutput != nullptr)
      return ledWizOutput->GetOutput();
   return nullptr;
}

void LedWizEquivalent::SetupOutputs(int numberOfOutputs)
{
   Log::Write(StringExtensions::Build("Setting up {0} outputs for LedWizEquivalent {1}", std::to_string(numberOfOutputs), std::to_string(m_ledWizNumber)));

   for (int i = 1; i <= numberOfOutputs; i++)
   {
      LedWizEquivalentOutput* output = m_outputs.GetOrCreateOutput(i);
      if (output != nullptr)
      {
         std::string outputName = "LedWiz " + std::to_string(m_ledWizNumber) + " Output " + std::to_string(i);
         output->SetOutputName(outputName);
      }
   }
}

void LedWizEquivalent::ResolveOutputs(Cabinet* cabinet)
{
   if (cabinet == nullptr)
      return;

   int resolvedCount = 0;

   for (LedWizEquivalentOutput* ledWizOutput : m_outputs)
   {
      if (ledWizOutput == nullptr)
         continue;

      const std::string& outputName = ledWizOutput->GetOutputName();
      if (outputName.empty())
         continue;

      IOutput* physicalOutput = nullptr;

      for (IOutputController* controller : *cabinet->GetOutputControllers())
      {
         if (controller == nullptr)
            continue;

         OutputControllerBase* baseController = dynamic_cast<OutputControllerBase*>(controller);
         if (baseController == nullptr)
            continue;

         OutputList* outputs = baseController->GetOutputs();
         if (outputs != nullptr)
         {
            for (IOutput* output : *outputs)
            {
               if (output != nullptr && output->GetName() == outputName)
               {
                  physicalOutput = output;
                  break;
               }
            }
         }

         if (physicalOutput != nullptr)
            break;
      }

      if (physicalOutput != nullptr)
      {
         ledWizOutput->SetOutput(physicalOutput);
         resolvedCount++;
      }
      else
      {
         Log::Warning(StringExtensions::Build("LedWizEquivalent {0}: Could not resolve output '{1}'", std::to_string(m_ledWizNumber), outputName));
      }
   }

   Log::Write(StringExtensions::Build(
      "LedWizEquivalent {0}: Resolved {1} of {2} outputs", std::to_string(m_ledWizNumber), std::to_string(resolvedCount), std::to_string(static_cast<int>(m_outputs.size()))));
}

}