#pragma once

#include "../generic/NamedItemList.h"
#include "FastImage.h"

namespace DOF
{

class FastImageList : public NamedItemList<FastImage>
{
public:
   FastImageList();
   virtual ~FastImageList();

   FastImage& operator[](const std::string& name);

   bool GetDontAddIfMissing() const { return m_dontAddIfMissing; }
   void SetDontAddIfMissing(bool value) { m_dontAddIfMissing = value; }

private:
   bool m_dontAddIfMissing;
};

}