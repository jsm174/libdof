#pragma once

namespace DOF
{

class AlphaMappingTable
{
public:
   static float AlphaMapping[256][256];

private:
   static bool s_initialized;
   static void Initialize();

   AlphaMappingTable() = delete;
   ~AlphaMappingTable() = delete;
   AlphaMappingTable(const AlphaMappingTable&) = delete;
   AlphaMappingTable& operator=(const AlphaMappingTable&) = delete;

   friend class AlphaMappingTableInitializer;
};

class AlphaMappingTableInitializer
{
public:
   AlphaMappingTableInitializer();
};

static AlphaMappingTableInitializer g_alphaMappingTableInitializer;

}