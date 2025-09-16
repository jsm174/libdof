#pragma once

#include <functional>

namespace DOF
{

class Action
{
public:
   Action()
      : m_target(nullptr)
      , m_method(nullptr)
      , m_hasParameter(false)
      , m_parameter(nullptr)
   {
   }

   template <typename T>
   Action(T* instance, void (T::*method)())
      : m_target(static_cast<void*>(instance))
      , m_method(reinterpret_cast<void*&>(method))
      , m_hasParameter(false)
      , m_parameter(nullptr)
      , m_function([instance, method]() { (instance->*method)(); })
   {
   }

   template <typename T, typename P>
   Action(T* instance, void (T::*method)(P*), P* parameter)
      : m_target(static_cast<void*>(instance))
      , m_method(reinterpret_cast<void*&>(method))
      , m_hasParameter(true)
      , m_parameter(static_cast<void*>(parameter))
      , m_parameterizedFunction([instance, method](void* param) { (instance->*method)(static_cast<P*>(param)); })
   {
   }

   Action(const Action& other) = default;
   Action& operator=(const Action& other) = default;

   void operator()() const
   {
      if (!m_hasParameter && m_function)
         m_function();
   }

   void operator()(void* parameter) const
   {
      if (m_hasParameter && m_parameterizedFunction)
         m_parameterizedFunction(parameter);
   }

   bool operator==(const Action& other) const { return m_target == other.m_target && m_method == other.m_method && m_hasParameter == other.m_hasParameter; }

   bool operator!=(const Action& other) const { return !(*this == other); }

   explicit operator bool() const { return m_target && ((!m_hasParameter && m_function) || (m_hasParameter && m_parameterizedFunction)); }

   bool HasParameter() const { return m_hasParameter; }

private:
   void* m_target;
   void* m_method;
   bool m_hasParameter;
   void* m_parameter;
   std::function<void()> m_function;
   std::function<void(void*)> m_parameterizedFunction;
};

}