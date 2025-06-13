#pragma once

#include "DOF/DOF.h"
#include <string>
#include <unordered_map>

namespace DOF
{

class Table;
class Cabinet;
class LedControlConfigList;
class TableConfig;
class IToy;
class IEffect;

class Configurator
{
public:
   Configurator();
   ~Configurator();

   int GetEffectMinDurationMs() const { return m_effectMinDurationMs; }
   void SetEffectMinDurationMs(int value) { m_effectMinDurationMs = value; }
   int GetEffectRGBMinDurationMs() const { return m_effectRGBMinDurationMs; }
   void SetEffectRGBMinDurationMs(int value) { m_effectRGBMinDurationMs = value; }
   void Setup(LedControlConfigList* ledControlConfigList, Table* table, Cabinet* cabinet, const std::string& romName);

private:
   int m_effectMinDurationMs;
   int m_effectRGBMinDurationMs;

   void SetupTable(Table* table, const std::unordered_map<int, TableConfig*>& tableConfigDict, const std::unordered_map<int, std::unordered_map<int, IToy*>>& toyAssignments,
      const std::string& iniFilePath);
   std::unordered_map<int, std::unordered_map<int, IToy*>> SetupCabinet(const std::unordered_map<int, TableConfig*>& tableConfigDict, Cabinet* cabinet);

   void MakeEffectNameUnique(IEffect* effect, Table* table);
   void AssignEffectToTableElements(Table* table, const std::vector<std::string>& tableElementDescriptors, IEffect* effect);
};

}