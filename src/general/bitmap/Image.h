#pragma once

#include <string>
#include <vector>

namespace DOF
{

class Image
{
public:
   Image();
   ~Image();

   static Image FromFile(const std::string& filename);

   int GetFrameCount() const;
   void SelectActiveFrame(int frameNumber);
   int GetWidth() const;
   int GetHeight() const;
   unsigned char* GetPixelData() const;
   void Dispose();

private:
   std::vector<unsigned char*> m_frameData;
   std::vector<int> m_frameDelays;
   int m_width;
   int m_height;
   int m_channels;
   int m_frameCount;
   int m_currentFrame;
   bool m_disposed;

   void LoadFromFile(const std::string& filename);
};

}