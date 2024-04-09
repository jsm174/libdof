#pragma once

#include "DOF/DOF.h"

namespace DOF
{

class Cabinet;
class Table;

class Pinball
{
 public:
  static Pinball* GetInstance();

  void Setup(const char* szTableFilename, const char* szRomName);
  void Init();
  void ReceiveData(char type, int number, int value);
  void Finish();

 private:
  Pinball(){};
  ~Pinball(){};

  static Pinball* m_pInstance;

  Cabinet* m_pCabinet;
  Table* m_pTable;
};

}  // namespace DOF
