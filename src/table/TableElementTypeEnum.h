
#pragma once

#include "DOF/DOF.h"

namespace DOF
{

enum class TableElementTypeEnum
{
   Unknown = '?',
   Lamp = 'L',
   Switch = 'W',
   Solenoid = 'S',
   GIString = 'G',
   Mech = 'M',
   GetMech = 'N',
   EMTable = 'E',
   LED = 'D',
   Score = 'C',
   ScoreDigit = 'B',
   NamedElement = '$'
};

}