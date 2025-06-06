#include "InputQueue.h"

#include <stdexcept>

namespace DOF
{

InputQueue::InputQueue() { }

void InputQueue::Enqueue(char tableElementTypeChar, int number, int value)
{
   std::lock_guard<std::mutex> lock(queueLocker);
   this->push(TableElementData(tableElementTypeChar, number, value));
}

void InputQueue::Enqueue(const TableElementData& tableElementData)
{
   std::lock_guard<std::mutex> lock(queueLocker);
   this->push(tableElementData);
}

void InputQueue::Enqueue(const std::string& tableElementName, int value)
{
   if (tableElementName.empty())
      return;

   std::lock_guard<std::mutex> lock(queueLocker);
   this->push(TableElementData(tableElementName, value));
}

TableElementData InputQueue::Dequeue()
{
   std::lock_guard<std::mutex> lock(queueLocker);
   if (this->empty())
      throw std::runtime_error("Queue is empty");
   TableElementData frontElement = this->front();
   this->pop();
   return frontElement;
}

TableElementData InputQueue::Peek()
{
   std::lock_guard<std::mutex> lock(queueLocker);
   if (this->empty())
      throw std::runtime_error("Queue is empty");
   return this->front();
}

int InputQueue::Count()
{
   std::lock_guard<std::mutex> lock(queueLocker);
   return this->size();
}

void InputQueue::Clear()
{
   std::lock_guard<std::mutex> lock(queueLocker);
   std::queue<TableElementData> empty;
   std::swap(*this, empty);
}

} // namespace DOF