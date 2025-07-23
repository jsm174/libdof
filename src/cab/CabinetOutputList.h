#pragma once

#include "DOF/DOF.h"
#include "out/IOutput.h"

namespace DOF
{

class Cabinet;
class OutputControllerList;

class CabinetOutputList
{
public:
   CabinetOutputList(Cabinet* cabinet);
   ~CabinetOutputList();

   IOutput* GetByName(const std::string& name);
   const IOutput* GetByName(const std::string& name) const;
   bool Contains(const std::string& name) const;
   bool Contains(IOutput* output) const;
   IOutput* operator[](const std::string& name);
   const IOutput* operator[](const std::string& name) const;
   IOutput* operator[](int index);
   const IOutput* operator[](int index) const;
   int GetCount() const;
   void ConnectOutputsToControllers();
   void SetCabinet(Cabinet* cabinet) { m_cabinet = cabinet; }

   class Iterator
   {
   public:
      Iterator(CabinetOutputList* list, int index);
      IOutput* operator*();
      Iterator& operator++();
      bool operator!=(const Iterator& other) const;
      bool operator==(const Iterator& other) const;

   private:
      CabinetOutputList* m_list;
      int m_index;
   };

   class ConstIterator
   {
   public:
      ConstIterator(const CabinetOutputList* list, int index);
      const IOutput* operator*() const;
      ConstIterator& operator++();
      bool operator!=(const ConstIterator& other) const;
      bool operator==(const ConstIterator& other) const;

   private:
      const CabinetOutputList* m_list;
      int m_index;
   };

   Iterator begin();
   Iterator end();
   ConstIterator begin() const;
   ConstIterator end() const;
   ConstIterator cbegin() const;
   ConstIterator cend() const;

private:
   Cabinet* m_cabinet;
   OutputControllerList* GetOutputControllers() const;
};

}
