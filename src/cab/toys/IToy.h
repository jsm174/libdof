#pragma once

#include "DOF/DOF.h"

namespace DOF
{

class Cabinet;

class IToy
{
 public:
  IToy() {}
  ~IToy() {}

  const std::string& GetName() const { return m_szName; }
  void SetName(const std::string& szName) { m_szName = szName; }

  virtual void Init(Cabinet* pCabinet) = 0;
  virtual void Reset() = 0;
  virtual void Finish() = 0;

 private:
  std::string m_szName;
};

}  // namespace DOF
