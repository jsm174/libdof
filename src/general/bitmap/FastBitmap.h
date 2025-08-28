#pragma once

#include "PixelData.h"
#include "FastBitmapDataExtractModeEnum.h"
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
   FastBitmap(int width, int height, unsigned char* pixelData);
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


   FastBitmap GetScaled(int newWidth, int newHeight) const;
   FastBitmap GetCropped(int x, int y, int width, int height) const;

   FastBitmap GetClip(int resultWidth, int resultHeight, int sourceLeft = 0, int sourceTop = 0, int sourceWidth = -1, int sourceHeight = -1,
      FastBitmapDataExtractModeEnum dataExtractMode = FastBitmapDataExtractModeEnum::SinglePixelCenter) const;

   PixelData** GetPixels() const;

   static FastBitmap GetFrameFromAnimated(const std::vector<FastBitmap>& frames, int frameIndex);

private:
   int m_width;
   int m_height;
   std::vector<PixelData> m_pixels;

   bool IsValidCoordinate(int x, int y) const;
   int GetPixelIndex(int x, int y) const;
   void SetFrameSize(int width, int height);
};

}