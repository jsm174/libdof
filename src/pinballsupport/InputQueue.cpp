#include "InputQueue.h"
#include "../general/StringExtensions.h"

#include <stdexcept>

namespace DOF
{

InputQueue::InputQueue() { }

void InputQueue::Enqueue(char tableElementTypeChar, int number, int value) { Enqueue(TableElementData(tableElementTypeChar, number, value)); }

void InputQueue::Enqueue(const TableElementData& tableElementData)
{
   std::lock_guard<std::mutex> lock(m_queueLocker);
   this->push(tableElementData);
}

void InputQueue::Enqueue(const std::string& tableElementName, int value)
{
   if (StringExtensions::IsNullOrWhiteSpace(tableElementName))
      return;

   std::string cleanedName = StringExtensions::Replace(tableElementName, " ", "_");

   std::lock_guard<std::mutex> lock(m_queueLocker);
   this->push(TableElementData(cleanedName, value));
}

TableElementData InputQueue::Dequeue()
{
   std::lock_guard<std::mutex> lock(m_queueLocker);
   if (this->empty())
      throw std::runtime_error("Queue is empty");
   TableElementData frontElement = this->front();
   this->pop();
   return frontElement;
}

TableElementData InputQueue::Peek()
{
   std::lock_guard<std::mutex> lock(m_queueLocker);
   if (this->empty())
      throw std::runtime_error("Queue is empty");
   return this->front();
}

int InputQueue::Count()
{
   std::lock_guard<std::mutex> lock(m_queueLocker);
   return static_cast<int>(this->size());
}

void InputQueue::Clear()
{
   std::lock_guard<std::mutex> lock(m_queueLocker);
   std::queue<TableElementData> empty;
   std::swap(*this, empty);
}

}