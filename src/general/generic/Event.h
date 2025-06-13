#pragma once

#include <functional>
#include <vector>
#include <algorithm>

namespace DOF
{

template <typename TEventArgs> class Event
{
public:
   using EventHandler = std::function<void(void* sender, const TEventArgs& args)>;

   void operator+=(const EventHandler& handler) { m_handlers.push_back(handler); }

   void operator-=(const EventHandler& handler) { }

   void Invoke(void* sender, const TEventArgs& args)
   {
      for (const auto& handler : m_handlers)
      {
         if (handler)
            handler(sender, args);
      }
   }

   bool HasHandlers() const { return !m_handlers.empty(); }

private:
   std::vector<EventHandler> m_handlers;
};

}