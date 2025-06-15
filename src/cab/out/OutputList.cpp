#include "OutputList.h"
#include "Output.h"
#include <algorithm>

namespace DOF
{

OutputList::OutputList() { }

OutputList::~OutputList() { clear(); }

void OutputList::Add(IOutput* output)
{
   if (output && !Contains(output))
   {
      if (Contains(output->GetName()))
      {
         throw std::invalid_argument("Cant insert the IOutput named " + output->GetName() + ". The name does already exist in the collection.");
      }

      push_back(output);
      AddToNameDict(output);

      output->ValueChanged += [this](void* sender, const OutputEventArgs& args) { OutputValueChanged.Invoke(this, args); };
   }
}

bool OutputList::Remove(IOutput* output)
{
   if (!output)
      return false;

   auto it = std::find(begin(), end(), output);
   if (it != end())
   {
      RemoveFromNameDict(output);
      erase(it);
      return true;
   }
   return false;
}

bool OutputList::Remove(const std::string& name)
{
   IOutput* output = GetByName(name);
   if (output)
   {
      return Remove(output);
   }
   return false;
}

IOutput* OutputList::GetByName(const std::string& name)
{
   auto it = m_nameDict.find(name);
   return (it != m_nameDict.end()) ? it->second : nullptr;
}

const IOutput* OutputList::GetByName(const std::string& name) const
{
   auto it = m_nameDict.find(name);
   return (it != m_nameDict.end()) ? it->second : nullptr;
}

bool OutputList::Contains(const std::string& name) const { return m_nameDict.find(name) != m_nameDict.end(); }

bool OutputList::Contains(IOutput* output) const { return std::find(begin(), end(), output) != end(); }

IOutput* OutputList::operator[](const std::string& name)
{
   auto it = m_nameDict.find(name);
   if (it == m_nameDict.end())
   {
      throw std::out_of_range("Output with name '" + name + "' not found");
   }
   return it->second;
}

const IOutput* OutputList::operator[](const std::string& name) const
{
   auto it = m_nameDict.find(name);
   if (it == m_nameDict.end())
   {
      throw std::out_of_range("Output with name '" + name + "' not found");
   }
   return it->second;
}

IOutput* OutputList::operator[](size_t index)
{
   if (index >= size())
   {
      throw std::out_of_range("Index out of range");
   }
   return std::vector<IOutput*>::operator[](index);
}

const IOutput* OutputList::operator[](size_t index) const
{
   if (index >= size())
   {
      throw std::out_of_range("Index out of range");
   }
   return std::vector<IOutput*>::operator[](index);
}

IOutput* OutputList::FindByNumber(int number) const
{
   auto it = std::find_if(begin(), end(), [number](IOutput* output) { return output && output->GetNumber() == number; });
   return (it != end()) ? *it : nullptr;
}

void OutputList::UpdateNameDict()
{
   m_nameDict.clear();
   for (IOutput* output : *this)
   {
      if (output)
      {
         m_nameDict[output->GetName()] = output;
      }
   }
}

void OutputList::AddToNameDict(IOutput* output)
{
   if (output)
   {
      m_nameDict[output->GetName()] = output;
   }
}

void OutputList::RemoveFromNameDict(IOutput* output)
{
   if (output)
   {
      auto it = m_nameDict.find(output->GetName());
      if (it != m_nameDict.end() && it->second == output)
      {
         m_nameDict.erase(it);
      }
   }
}

XMLElement* OutputList::ToXml(XMLDocument& doc) const
{
   XMLElement* element = doc.NewElement(GetXmlElementName().c_str());
   return element;
}

bool OutputList::FromXml(const XMLElement* element)
{
   if (!element)
      return false;

   clear();
   m_nameDict.clear();
   return true;
}

}