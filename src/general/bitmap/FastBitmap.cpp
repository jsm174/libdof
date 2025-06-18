#include "FastBitmap.h"
#include "../StringExtensions.h"
#include "../../Log.h"
#include <fstream>
#include <algorithm>
#include <cstring>

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

FastBitmap::FastBitmap(const std::string& filename)
   : m_width(0)
   , m_height(0)
{
   LoadFromFile(filename);
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

bool FastBitmap::LoadFromFile(const std::string& filename)
{
   if (filename.empty())
      return false;

   std::string ext = StringExtensions::ToLower(filename);

   if (ext.ends_with(".bmp"))
   {
      return LoadBMP(filename);
   }
   else
   {

      return LoadSimpleBitmap(filename);
   }
}

bool FastBitmap::SaveToFile(const std::string& filename) const
{
   if (filename.empty() || !IsValid())
      return false;

   std::string ext = StringExtensions::ToLower(filename);

   if (ext.ends_with(".bmp"))
   {
      return SaveBMP(filename);
   }

   return false;
}

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

std::vector<FastBitmap> FastBitmap::LoadAnimatedBitmap(const std::string& filename)
{
   std::vector<FastBitmap> frames;

   FastBitmap bitmap(filename);
   if (bitmap.IsValid())
   {
      frames.push_back(bitmap);
   }

   return frames;
}

FastBitmap FastBitmap::GetFrameFromAnimated(const std::vector<FastBitmap>& frames, int frameIndex)
{
   if (frames.empty())
      return FastBitmap();

   int index = frameIndex % static_cast<int>(frames.size());
   return frames[index];
}

#pragma pack(push, 1)
struct BMPHeader
{
   uint16_t signature;
   uint32_t fileSize;
   uint32_t reserved;
   uint32_t dataOffset;
   uint32_t headerSize;
   uint32_t width;
   uint32_t height;
   uint16_t planes;
   uint16_t bitsPerPixel;
   uint32_t compression;
   uint32_t imageSize;
   uint32_t xPixelsPerMeter;
   uint32_t yPixelsPerMeter;
   uint32_t colorsUsed;
   uint32_t colorsImportant;
};
#pragma pack(pop)

bool FastBitmap::LoadBMP(const std::string& filename)
{
   std::ifstream file(filename, std::ios::binary);
   if (!file.is_open())
   {
      Log::Warning(StringExtensions::Build("Could not open bitmap file: {0}", filename));
      return false;
   }

   BMPHeader header;
   file.read(reinterpret_cast<char*>(&header), sizeof(header));

   if (header.signature != 0x4D42)
   {
      Log::Warning(StringExtensions::Build("Invalid BMP signature in file: {0}", filename));
      return false;
   }

   if (header.bitsPerPixel != 24 && header.bitsPerPixel != 32)
   {
      Log::Warning(StringExtensions::Build("Unsupported BMP format (only 24/32 bit supported): {0}", filename));
      return false;
   }

   m_width = header.width;
   m_height = header.height;
   m_pixels.resize(m_width * m_height);

   file.seekg(header.dataOffset);

   int bytesPerPixel = header.bitsPerPixel / 8;
   int rowSize = ((m_width * bytesPerPixel + 3) / 4) * 4;

   std::vector<uint8_t> rowData(rowSize);

   for (int y = m_height - 1; y >= 0; y--)
   {
      file.read(reinterpret_cast<char*>(rowData.data()), rowSize);

      for (int x = 0; x < m_width; x++)
      {
         int pixelOffset = x * bytesPerPixel;

         PixelData pixel;
         pixel.blue = rowData[pixelOffset];
         pixel.green = rowData[pixelOffset + 1];
         pixel.red = rowData[pixelOffset + 2];
         pixel.alpha = (bytesPerPixel == 4) ? rowData[pixelOffset + 3] : 255;

         SetPixel(x, y, pixel);
      }
   }

   return true;
}

bool FastBitmap::LoadSimpleBitmap(const std::string& filename)
{
   m_width = 16;
   m_height = 16;
   m_pixels.resize(m_width * m_height);

   for (int y = 0; y < m_height; y++)
   {
      for (int x = 0; x < m_width; x++)
      {
         uint8_t value = (uint8_t)((x + y) * 255 / (m_width + m_height - 2));
         SetPixel(x, y, PixelData(value, value, value, 255));
      }
   }

   Log::Write(StringExtensions::Build("Loaded simple test pattern for: {0}", filename));
   return true;
}

bool FastBitmap::SaveBMP(const std::string& filename) const
{
   if (!IsValid())
      return false;

   std::ofstream file(filename, std::ios::binary);
   if (!file.is_open())
      return false;

   int bytesPerPixel = 3;
   int rowSize = ((m_width * bytesPerPixel + 3) / 4) * 4;
   int imageSize = rowSize * m_height;

   BMPHeader header = {};
   header.signature = 0x4D42;
   header.fileSize = sizeof(BMPHeader) + imageSize;
   header.dataOffset = sizeof(BMPHeader);
   header.headerSize = 40;
   header.width = m_width;
   header.height = m_height;
   header.planes = 1;
   header.bitsPerPixel = 24;
   header.compression = 0;
   header.imageSize = imageSize;

   file.write(reinterpret_cast<const char*>(&header), sizeof(header));

   std::vector<uint8_t> rowData(rowSize, 0);

   for (int y = m_height - 1; y >= 0; y--)
   {
      for (int x = 0; x < m_width; x++)
      {
         PixelData pixel = GetPixel(x, y);
         int pixelOffset = x * bytesPerPixel;

         rowData[pixelOffset] = pixel.blue;
         rowData[pixelOffset + 1] = pixel.green;
         rowData[pixelOffset + 2] = pixel.red;
      }

      file.write(reinterpret_cast<const char*>(rowData.data()), rowSize);
   }

   return true;
}

}