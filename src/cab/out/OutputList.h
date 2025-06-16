#pragma once

#include "DOF/DOF.h"
#include "IOutput.h"
#include "../../general/generic/Event.h"
#include "../../general/generic/IXmlSerializable.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace DOF
{

class OutputList : public std::vector<IOutput*>, public IXmlSerializable
{
public:
   OutputList();
   virtual ~OutputList();

   IOutput* GetByName(const std::string& name);
   const IOutput* GetByName(const std::string& name) const;
   bool Contains(const std::string& name) const;
   bool Contains(IOutput* output) const;

   IOutput* operator[](const std::string& name);
   const IOutput* operator[](const std::string& name) const;
   IOutput* operator[](size_t index);
   const IOutput* operator[](size_t index) const;

   IOutput* FindByNumber(int number) const;

   void Add(IOutput* output);
   bool Remove(IOutput* output);
   bool Remove(const std::string& name);

   virtual tinyxml2::XMLElement* ToXml(tinyxml2::XMLDocument& doc) const override;
   virtual bool FromXml(const tinyxml2::XMLElement* element) override;
   virtual std::string GetXmlElementName() const override { return "Outputs"; }

   Event<OutputEventArgs> OutputValueChanged;

private:
   std::unordered_map<std::string, IOutput*> m_nameDict;

   void UpdateNameDict();
   void AddToNameDict(IOutput* output);
   void RemoveFromNameDict(IOutput* output);
};

}