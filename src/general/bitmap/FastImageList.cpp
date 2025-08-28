#include "FastImageList.h"
#include "../../Log.h"
#include "../StringExtensions.h"

namespace DOF
{

FastImageList::FastImageList()
   : m_dontAddIfMissing(false)
{
}

FastImageList::~FastImageList() { }

FastImage& FastImageList::operator[](const std::string& name)
{
   FastImage* existing = NamedItemList<FastImage>::operator[](name);
   if (existing != nullptr)
   {
      return *existing;
   }

   if (!m_dontAddIfMissing)
   {
      try
      {
         FastImage* f = new FastImage(name);
         Add(f);
         return *f;
      }
      catch (const std::exception& e)
      {
         throw std::runtime_error(StringExtensions::Build("Could not add file {0} to the FastImageList.", name));
      }
   }
   else
   {
      throw std::runtime_error("Item not found and DontAddIfMissing is true");
   }
}

}