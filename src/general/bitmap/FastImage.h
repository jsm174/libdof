#pragma once

#include "FastBitmap.h"
#include "../generic/NamedItemBase.h"
#include <unordered_map>

namespace DOF
{

class FastImage : public NamedItemBase
{
public:
   FastImage();
   FastImage(const std::string& name);
   virtual ~FastImage();

   const std::unordered_map<int, FastBitmap>& GetFrames() const { return m_frames; }
   void LoadImageFile(const std::string& imageFilePath);

protected:
   virtual void AfterNameChange(const std::string& oldName, const std::string& newName) override;

private:
   std::unordered_map<int, FastBitmap> m_frames;
};

}