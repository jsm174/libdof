#include "FastImage.h"
#include "FastBitmap.h"
#include "Image.h"
#include "../../Log.h"
#include "../StringExtensions.h"

namespace DOF
{

FastImage::FastImage() { }

FastImage::FastImage(const std::string& name)
   : NamedItemBase()
{
   SetName(name);
}

FastImage::~FastImage() { }

void FastImage::LoadImageFile(const std::string& imageFilePath)
{
   m_frames.clear();

   Image img = Image::FromFile(imageFilePath);

   int frameCount = img.GetFrameCount();

   for (int frameNumber = 0; frameNumber < frameCount; frameNumber++)
   {
      img.SelectActiveFrame(frameNumber);

      FastBitmap f(img.GetWidth(), img.GetHeight(), img.GetPixelData());

      m_frames[frameNumber] = f;
   }

   img.Dispose();

   Log::Debug(StringExtensions::Build("FastImage::LoadImageFile: Loaded {0} frames from {1}", std::to_string(frameCount), imageFilePath));
}

void FastImage::AfterNameChange(const std::string& oldName, const std::string& newName) { LoadImageFile(newName); }

}