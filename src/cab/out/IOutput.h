#pragma once

#include "DOF/DOF.h"
#include "../../general/generic/Event.h"
#include "../../general/generic/NamedItemBase.h"

namespace DOF
{

class OutputEventArgs
{
public:
   OutputEventArgs(class IOutput* output)
      : m_output(output)
   {
   }
   OutputEventArgs()
      : m_output(nullptr)
   {
   }

   IOutput* GetOutput() const { return m_output; }
   void SetOutput(IOutput* output) { m_output = output; }

   uint8_t GetValue() const;
   const std::string& GetName() const;

private:
   IOutput* m_output;
   static const std::string s_emptyString;
};

class IOutput : public NamedItemBase
{
public:
   IOutput();
   virtual ~IOutput();

   virtual uint8_t GetOutput() const { return m_value; }
   virtual void SetOutput(uint8_t value) { m_value = value; }
   int GetNumber() const { return m_number; }
   void SetNumber(int number) { m_number = number; }

   Event<OutputEventArgs> ValueChanged;

protected:
   virtual void OnValueChanged();

private:
   uint8_t m_value;
   int m_number;
};

}
