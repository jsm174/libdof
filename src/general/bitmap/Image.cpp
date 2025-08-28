#include "Image.h"
#include "../../Log.h"
#include "../StringExtensions.h"
#include <algorithm>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace DOF
{

Image::Image()
   : m_width(0)
   , m_height(0)
   , m_channels(0)
   , m_frameCount(0)
   , m_currentFrame(0)
   , m_disposed(false)
{
}

Image::~Image() { Dispose(); }

Image Image::FromFile(const std::string& filename)
{
   Image image;
   image.LoadFromFile(filename);
   return image;
}

void Image::LoadFromFile(const std::string& filename)
{
   FILE* file = fopen(filename.c_str(), "rb");
   if (!file)
   {
      Log::Warning(StringExtensions::Build("Image::LoadFromFile: Could not open file {0}", filename));
      return;
   }

   fseek(file, 0, SEEK_END);
   size_t fileSize = ftell(file);
   fseek(file, 0, SEEK_SET);

   std::vector<unsigned char> buffer(fileSize);
   size_t bytesRead = fread(buffer.data(), 1, fileSize, file);
   fclose(file);

   if (bytesRead != fileSize)
   {
      Log::Warning("Image::LoadFromFile - Failed to read complete file: " + filename);
      return;
   }

   size_t dotPos = filename.rfind('.');
   std::string extension;
   if (dotPos != std::string::npos)
   {
      extension = filename.substr(dotPos + 1);
      std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
   }

   if (extension == "gif")
   {
      int* delays = nullptr;
      int frameCount;
      int channels;
      unsigned char* gifData = stbi_load_gif_from_memory(buffer.data(), static_cast<int>(fileSize), &delays, &m_width, &m_height, &frameCount, &channels, 4);

      if (gifData != nullptr && frameCount > 1)
      {
         m_channels = 4;
         m_frameCount = frameCount;
         m_currentFrame = 0;

         size_t frameSize = m_width * m_height * 4;
         for (int i = 0; i < frameCount; i++)
         {
            unsigned char* frameData = new unsigned char[frameSize];
            memcpy(frameData, gifData + (i * frameSize), frameSize);
            m_frameData.push_back(frameData);
            m_frameDelays.push_back(delays[i]);
         }

         stbi_image_free(gifData);
         if (delays)
            delete[] delays;

         Log::Debug(StringExtensions::Build("Image::LoadFromFile: Loaded {0} frames from animated GIF {1}", std::to_string(frameCount), filename));
      }
      else
      {
         if (gifData)
            stbi_image_free(gifData);
         if (delays)
            delete[] delays;
         Log::Warning(StringExtensions::Build("Image::LoadFromFile: Failed to load GIF {0}", filename));
      }
   }
   else
   {
      int channels;
      unsigned char* imageData = stbi_load_from_memory(buffer.data(), static_cast<int>(fileSize), &m_width, &m_height, &channels, 4);

      if (imageData != nullptr)
      {
         m_channels = 4;
         m_frameCount = 1;
         m_currentFrame = 0;

         size_t frameSize = m_width * m_height * 4;
         unsigned char* frameData = new unsigned char[frameSize];
         memcpy(frameData, imageData, frameSize);
         m_frameData.push_back(frameData);
         m_frameDelays.push_back(0);

         stbi_image_free(imageData);

         Log::Debug(StringExtensions::Build("Image::LoadFromFile: Loaded single frame from {0}", filename));
      }
      else
      {
         Log::Warning(StringExtensions::Build("Image::LoadFromFile: Failed to load image {0}", filename));
      }
   }
}

int Image::GetFrameCount() const { return m_frameCount; }

void Image::SelectActiveFrame(int frameNumber)
{
   if (frameNumber >= 0 && frameNumber < m_frameCount)
   {
      m_currentFrame = frameNumber;
   }
}

int Image::GetWidth() const { return m_width; }

int Image::GetHeight() const { return m_height; }

unsigned char* Image::GetPixelData() const
{
   if (m_currentFrame >= 0 && m_currentFrame < static_cast<int>(m_frameData.size()))
   {
      return m_frameData[m_currentFrame];
   }
   return nullptr;
}

void Image::Dispose()
{
   if (!m_disposed)
   {
      for (unsigned char* frameData : m_frameData)
      {
         delete[] frameData;
      }
      m_frameData.clear();
      m_frameDelays.clear();
      m_disposed = true;
   }
}

}