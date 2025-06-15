#pragma once

#include "PixelData.h"
#include <string>
#include <vector>
#include <memory>

namespace DOF
{

class FastBitmap
{
public:
   FastBitmap();
   FastBitmap(int width, int height);
   FastBitmap(const std::string& filename);
   ~FastBitmap();


   FastBitmap(const FastBitmap& other);
   FastBitmap& operator=(const FastBitmap& other);


   int GetWidth() const { return m_width; }
   int GetHeight() const { return m_height; }
   bool IsValid() const { return m_width > 0 && m_height > 0 && !m_pixels.empty(); }


   PixelData GetPixel(int x, int y) const;
   void SetPixel(int x, int y, const PixelData& pixel);


   void Clear(const PixelData& color = PixelData(0, 0, 0, 0));
   void Fill(const PixelData& color);


   bool LoadFromFile(const std::string& filename);
   bool SaveToFile(const std::string& filename) const;


   FastBitmap GetScaled(int newWidth, int newHeight) const;
   FastBitmap GetCropped(int x, int y, int width, int height) const;


   static std::vector<FastBitmap> LoadAnimatedBitmap(const std::string& filename);
   static FastBitmap GetFrameFromAnimated(const std::vector<FastBitmap>& frames, int frameIndex);

private:
   int m_width;
   int m_height;
   std::vector<PixelData> m_pixels;

   bool IsValidCoordinate(int x, int y) const;
   int GetPixelIndex(int x, int y) const;


   bool LoadBMP(const std::string& filename);
   bool LoadSimpleBitmap(const std::string& filename);
   bool SaveBMP(const std::string& filename) const;
};

}