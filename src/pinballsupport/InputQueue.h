#pragma once

#include <mutex>
#include <queue>

#include "../table/TableElementData.h"
#include "DOF/DOF.h"

namespace DOF
{
class InputQueue : public std::queue<TableElementData>
{
public:
   InputQueue();

   void Enqueue(char tableElementTypeChar, int number, int value);
   void Enqueue(const TableElementData& tableElementData);
   void Enqueue(const std::string& tableElementName, int value);
   TableElementData Dequeue();
   TableElementData Peek();
   int Count();
   void Clear();

private:
   std::mutex m_queueLocker;
};

}