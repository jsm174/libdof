#pragma once

#include "NamedItemBase.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <stdexcept>

namespace DOF
{

template <typename T> class NamedItemList : public std::vector<T*>
{
private:
   std::unordered_map<std::string, T*> m_nameDict;

public:
   NamedItemList() = default;
   virtual ~NamedItemList() { Clear(); }

   T* operator[](const std::string& name) const
   {
      auto it = m_nameDict.find(name);
      return (it != m_nameDict.end()) ? it->second : nullptr;
   }

   bool Contains(const std::string& name) const { return m_nameDict.find(name) != m_nameDict.end(); }

   void Add(T* item)
   {
      if (!item)
         return;

      if (Contains(item->GetName()))
      {
         throw std::runtime_error("Item with name already exists in collection");
      }

      this->push_back(item);
      m_nameDict[item->GetName()] = item;
   }

   bool Remove(const std::string& name)
   {
      auto it = std::find_if(this->begin(), this->end(), [&name](T* item) { return item && item->GetName() == name; });

      if (it != this->end())
      {
         delete *it;
         this->erase(it);
         m_nameDict.erase(name);
         return true;
      }
      return false;
   }

   void Clear()
   {
      for (T* item : *this)
      {
         delete item;
      }
      this->clear();
      m_nameDict.clear();
   }

   bool IsEmpty() const { return this->empty(); }
};

}