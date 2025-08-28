#include "FastBitmap.h"
#include "../StringExtensions.h"
#include "../MathExtensions.h"
#include "../../Log.h"
#include <fstream>
#include <algorithm>
#include <cstring>
#include <climits>
#include <vector>
#include <cstdio>


namespace DOF
{

FastBitmap::FastBitmap()
   : m_width(0)
   , m_height(0)
{
}

FastBitmap::FastBitmap(int width, int height)
   : m_width(width)
   , m_height(height)
{
   if (width > 0 && height > 0)
   {
      m_pixels.resize(width * height);
      Clear();
   }
}

FastBitmap::FastBitmap(int width, int height, unsigned char* pixelData)
   : m_width(width)
   , m_height(height)
{
   if (width > 0 && height > 0 && pixelData != nullptr)
   {
      m_pixels.resize(width * height);
      for (int i = 0; i < width * height; i++)
      {
         int dataIndex = i * 4;
         m_pixels[i] = PixelData(pixelData[dataIndex], pixelData[dataIndex + 1], pixelData[dataIndex + 2], pixelData[dataIndex + 3]);
      }
   }
}


FastBitmap::~FastBitmap() { }

FastBitmap::FastBitmap(const FastBitmap& other)
   : m_width(other.m_width)
   , m_height(other.m_height)
   , m_pixels(other.m_pixels)
{
}

FastBitmap& FastBitmap::operator=(const FastBitmap& other)
{
   if (this != &other)
   {
      m_width = other.m_width;
      m_height = other.m_height;
      m_pixels = other.m_pixels;
   }
   return *this;
}

bool FastBitmap::IsValidCoordinate(int x, int y) const { return x >= 0 && x < m_width && y >= 0 && y < m_height; }

int FastBitmap::GetPixelIndex(int x, int y) const { return y * m_width + x; }

PixelData FastBitmap::GetPixel(int x, int y) const
{
   if (!IsValidCoordinate(x, y))
      return PixelData(0, 0, 0, 0);

   return m_pixels[GetPixelIndex(x, y)];
}

void FastBitmap::SetPixel(int x, int y, const PixelData& pixel)
{
   if (IsValidCoordinate(x, y))
   {
      m_pixels[GetPixelIndex(x, y)] = pixel;
   }
}

void FastBitmap::Clear(const PixelData& color) { std::fill(m_pixels.begin(), m_pixels.end(), color); }

void FastBitmap::Fill(const PixelData& color) { Clear(color); }


FastBitmap FastBitmap::GetScaled(int newWidth, int newHeight) const
{
   if (!IsValid() || newWidth <= 0 || newHeight <= 0)
      return FastBitmap();

   FastBitmap scaled(newWidth, newHeight);

   for (int y = 0; y < newHeight; y++)
   {
      for (int x = 0; x < newWidth; x++)
      {
         int srcX = (x * m_width) / newWidth;
         int srcY = (y * m_height) / newHeight;

         srcX = std::clamp(srcX, 0, m_width - 1);
         srcY = std::clamp(srcY, 0, m_height - 1);

         scaled.SetPixel(x, y, GetPixel(srcX, srcY));
      }
   }

   return scaled;
}

FastBitmap FastBitmap::GetCropped(int x, int y, int width, int height) const
{
   if (!IsValid() || width <= 0 || height <= 0)
      return FastBitmap();

   FastBitmap cropped(width, height);

   for (int dy = 0; dy < height; dy++)
   {
      for (int dx = 0; dx < width; dx++)
      {
         int srcX = x + dx;
         int srcY = y + dy;

         if (IsValidCoordinate(srcX, srcY))
         {
            cropped.SetPixel(dx, dy, GetPixel(srcX, srcY));
         }
         else
         {
            cropped.SetPixel(dx, dy, PixelData(0, 0, 0, 0));
         }
      }
   }

   return cropped;
}


FastBitmap FastBitmap::GetFrameFromAnimated(const std::vector<FastBitmap>& frames, int frameIndex)
{
   if (frames.empty())
      return FastBitmap();

   int index = frameIndex % static_cast<int>(frames.size());
   return frames[index];
}

void FastBitmap::SetFrameSize(int width, int height)
{
   m_width = MathExtensions::Limit(width, 0, INT_MAX);
   m_height = MathExtensions::Limit(height, 0, INT_MAX);
   if (m_width > 0 && m_height > 0)
   {
      m_pixels.resize(m_width * m_height);
      Clear();
   }
   else
   {
      m_pixels.clear();
   }
}

FastBitmap FastBitmap::GetClip(int resultWidth, int resultHeight, int sourceLeft, int sourceTop, int sourceWidth, int sourceHeight, FastBitmapDataExtractModeEnum dataExtractMode) const
{
   sourceLeft = MathExtensions::Limit(sourceLeft, 0, m_width - 1);
   sourceTop = MathExtensions::Limit(sourceTop, 0, m_height - 1);
   sourceWidth = (sourceWidth < 0) ? m_width - sourceLeft : sourceWidth;
   sourceHeight = (sourceHeight < 0) ? m_height - sourceTop : sourceHeight;
   resultWidth = MathExtensions::Limit(resultWidth, 0, INT_MAX);
   resultHeight = MathExtensions::Limit(resultHeight, 0, INT_MAX);

   FastBitmap result;
   result.SetFrameSize(resultWidth, resultHeight);

   switch (dataExtractMode)
   {
   case FastBitmapDataExtractModeEnum::BlendPixels:
   {
      float red = 0.0f;
      float green = 0.0f;
      float blue = 0.0f;
      float alpha = 0.0f;
      float weight = 0.0f;

      float pixelSourceWidth = static_cast<float>(sourceWidth) / resultWidth;
      float pixelSourceHeight = static_cast<float>(sourceHeight) / resultHeight;
      float pixelSourceCount = pixelSourceWidth * pixelSourceHeight;

      for (int y = 0; y < resultHeight; y++)
      {
         float pixelSourceTop = sourceTop + y * pixelSourceHeight;
         float pixelSourceBottom = pixelSourceTop + pixelSourceHeight;

         for (int x = 0; x < resultWidth; x++)
         {
            int psr = 0;
            int psb = 0;
            float pixelSourceLeft = sourceLeft + x * pixelSourceWidth;
            float pixelSourceRight = pixelSourceLeft + pixelSourceWidth;
            red = 0.0f;
            green = 0.0f;
            blue = 0.0f;
            alpha = 0.0f;

            if (!MathExtensions::IsIntegral(pixelSourceTop))
            {
               if (!MathExtensions::IsIntegral(pixelSourceLeft))
               {
                  PixelData pd = GetPixel(static_cast<int>(MathExtensions::Floor(pixelSourceLeft)), static_cast<int>(MathExtensions::Floor(pixelSourceTop)));
                  weight = (MathExtensions::Ceiling(pixelSourceTop) - pixelSourceTop) * (MathExtensions::Ceiling(pixelSourceLeft) - pixelSourceLeft);
                  red += pd.red * weight;
                  green += pd.green * weight;
                  blue += pd.blue * weight;
                  alpha += pd.alpha * weight;
               }

               psr = static_cast<int>(MathExtensions::Floor(pixelSourceRight));
               weight = (MathExtensions::Ceiling(pixelSourceTop) - pixelSourceTop);
               for (int xs = static_cast<int>(MathExtensions::Ceiling(pixelSourceLeft)); xs < psr; xs++)
               {
                  PixelData pd = GetPixel(xs, static_cast<int>(MathExtensions::Floor(pixelSourceTop)));
                  red += pd.red * weight;
                  green += pd.green * weight;
                  blue += pd.blue * weight;
                  alpha += pd.alpha * weight;
               }

               if (!MathExtensions::IsIntegral(pixelSourceRight))
               {
                  weight = (MathExtensions::Ceiling(pixelSourceTop) - pixelSourceTop) * (pixelSourceRight - MathExtensions::Floor(pixelSourceRight));
                  PixelData pd = GetPixel(static_cast<int>(MathExtensions::Floor(pixelSourceRight)), static_cast<int>(MathExtensions::Floor(pixelSourceTop)));
                  red += pd.red * weight;
                  green += pd.green * weight;
                  blue += pd.blue * weight;
                  alpha += pd.alpha * weight;
               }
            }

            psb = static_cast<int>(MathExtensions::Floor(pixelSourceBottom));
            psr = static_cast<int>(MathExtensions::Floor(pixelSourceRight));
            for (int ys = static_cast<int>(MathExtensions::Ceiling(pixelSourceTop)); ys < psb; ys++)
            {
               if (!MathExtensions::IsIntegral(pixelSourceLeft))
               {
                  PixelData pd = GetPixel(static_cast<int>(MathExtensions::Floor(pixelSourceLeft)), ys);
                  weight = (MathExtensions::Ceiling(pixelSourceLeft) - pixelSourceLeft);
                  red += pd.red * weight;
                  green += pd.green * weight;
                  blue += pd.blue * weight;
                  alpha += pd.alpha * weight;
               }

               for (int xs = static_cast<int>(MathExtensions::Ceiling(pixelSourceLeft)); xs < psr; xs++)
               {
                  PixelData pd = GetPixel(xs, ys);
                  red += pd.red;
                  green += pd.green;
                  blue += pd.blue;
                  alpha += pd.alpha;
               }

               if (!MathExtensions::IsIntegral(pixelSourceRight))
               {
                  weight = (pixelSourceRight - psr);
                  PixelData pd = GetPixel(psr, ys);
                  red += pd.red * weight;
                  green += pd.green * weight;
                  blue += pd.blue * weight;
                  alpha += pd.alpha * weight;
               }
            }

            if (!MathExtensions::IsIntegral(pixelSourceBottom))
            {
               psb = static_cast<int>(MathExtensions::Floor(pixelSourceBottom));
               psr = static_cast<int>(MathExtensions::Floor(pixelSourceRight));

               if (!MathExtensions::IsIntegral(pixelSourceLeft))
               {
                  PixelData pd = GetPixel(static_cast<int>(MathExtensions::Floor(pixelSourceLeft)), psb);
                  weight = (pixelSourceBottom - psb) * (MathExtensions::Ceiling(pixelSourceLeft) - pixelSourceLeft);
                  red += pd.red * weight;
                  green += pd.green * weight;
                  blue += pd.blue * weight;
                  alpha += pd.alpha * weight;
               }

               weight = (pixelSourceBottom - psb);
               for (int xs = static_cast<int>(MathExtensions::Ceiling(pixelSourceLeft)); xs < psr; xs++)
               {
                  PixelData pd = GetPixel(xs, psb);
                  red += pd.red * weight;
                  green += pd.green * weight;
                  blue += pd.blue * weight;
                  alpha += pd.alpha * weight;
               }

               if (!MathExtensions::IsIntegral(pixelSourceRight))
               {
                  weight = (pixelSourceBottom - psb) * (pixelSourceRight - psr);
                  PixelData pd = GetPixel(psr, psb);
                  red += pd.red * weight;
                  green += pd.green * weight;
                  blue += pd.blue * weight;
                  alpha += pd.alpha * weight;
               }
            }

            result.SetPixel(x, y,
               PixelData(static_cast<uint8_t>(MathExtensions::Limit(red / pixelSourceCount, 0.0f, 255.0f)),
                  static_cast<uint8_t>(MathExtensions::Limit(green / pixelSourceCount, 0.0f, 255.0f)), static_cast<uint8_t>(MathExtensions::Limit(blue / pixelSourceCount, 0.0f, 255.0f)),
                  static_cast<uint8_t>(MathExtensions::Limit(alpha / pixelSourceCount, 0.0f, 255.0f))));
         }
      }
      break;
   }

   case FastBitmapDataExtractModeEnum::SinglePixelTopLeft:
   case FastBitmapDataExtractModeEnum::SinglePixelCenter:
   default:
   {
      float xSource = 0.0f;
      float xSourceBase = 0.0f;
      float ySource = 0.0f;
      float xStep = static_cast<float>(sourceWidth) / resultWidth;
      float yStep = static_cast<float>(sourceHeight) / resultHeight;

      if (dataExtractMode == FastBitmapDataExtractModeEnum::SinglePixelCenter)
      {
         xSourceBase = xStep / 2;
         ySource = yStep / 2;
      }

      for (int y = 0; y < resultHeight; y++)
      {
         xSource = xSourceBase;
         for (int x = 0; x < resultWidth; x++)
         {
            result.SetPixel(x, y, GetPixel(MathExtensions::RoundToInt(xSource), MathExtensions::RoundToInt(ySource)));
            xSource += xStep;
         }
         ySource += yStep;
      }
      break;
   }
   }

   return result;
}

PixelData** FastBitmap::GetPixels() const
{
   if (!IsValid())
      return nullptr;

   PixelData** pixels = new PixelData*[m_width];
   for (int x = 0; x < m_width; x++)
   {
      pixels[x] = new PixelData[m_height];
      for (int y = 0; y < m_height; y++)
      {
         pixels[x][y] = GetPixel(x, y);
      }
   }
   return pixels;
}


}