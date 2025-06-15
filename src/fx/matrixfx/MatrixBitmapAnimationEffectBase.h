#pragma once

#include "MatrixBitmapEffectBase.h"
#include "MatrixAnimationStepDirectionEnum.h"
#include "AnimationBehaviourEnum.h"
#include "../../general/StringExtensions.h"
#include "../../Log.h"
#include "../../pinballsupport/AlarmHandler.h"
#include <chrono>
#include <vector>

namespace DOF
{

class TableElementData;

template <typename MatrixElementType> class MatrixBitmapAnimationEffectBase : public MatrixBitmapEffectBase<MatrixElementType>
{
public:
   MatrixBitmapAnimationEffectBase();
   virtual ~MatrixBitmapAnimationEffectBase() = default;

   int GetAnimationStepCount() const { return m_animationStepCount; }
   void SetAnimationStepCount(int value) { m_animationStepCount = MathExtensions::Limit(value, 1, 1000); }
   int GetAnimationStepSize() const { return m_animationStepSize; }
   void SetAnimationStepSize(int value) { m_animationStepSize = MathExtensions::Limit(value, 1, 100); }
   int GetAnimationFrameDurationMs() const { return m_animationFrameDurationMs; }
   void SetAnimationFrameDurationMs(int value) { m_animationFrameDurationMs = MathExtensions::Limit(value, 1, 10000); }
   MatrixAnimationStepDirectionEnum GetAnimationStepDirection() const { return m_animationStepDirection; }
   void SetAnimationStepDirection(MatrixAnimationStepDirectionEnum value) { m_animationStepDirection = value; }
   AnimationBehaviourEnum GetAnimationBehaviour() const { return m_animationBehaviour; }
   void SetAnimationBehaviour(AnimationBehaviourEnum value) { m_animationBehaviour = value; }
   virtual void Trigger(TableElementData* tableElementData) override;
   virtual void Finish() override;

protected:
   void StartAnimation(TableElementData* tableElementData);
   void StopAnimation();
   void AnimationStep();
   void RenderCurrentFrame(TableElementData* tableElementData);

   std::vector<FastBitmap> m_animationFrames;
   bool m_animationActive;
   int m_currentFrame;
   int m_currentStep;
   TableElementData* m_animationTableElementData;

private:
   int m_animationStepCount;
   int m_animationStepSize;
   int m_animationFrameDurationMs;
   MatrixAnimationStepDirectionEnum m_animationStepDirection;
   AnimationBehaviourEnum m_animationBehaviour;

   void LoadAnimationFrames();
   int CalculateFrameIndex() const;
};


template <typename MatrixElementType>
MatrixBitmapAnimationEffectBase<MatrixElementType>::MatrixBitmapAnimationEffectBase()
   : m_animationActive(false)
   , m_currentFrame(0)
   , m_currentStep(0)
   , m_animationTableElementData(nullptr)
   , m_animationStepCount(1)
   , m_animationStepSize(1)
   , m_animationFrameDurationMs(100)
   , m_animationStepDirection(MatrixAnimationStepDirectionEnum::Frame)
   , m_animationBehaviour(AnimationBehaviourEnum::Loop)
{
}

template <typename MatrixElementType> void MatrixBitmapAnimationEffectBase<MatrixElementType>::Trigger(TableElementData* tableElementData)
{
   if (this->m_matrixLayer != nullptr)
   {
      if (tableElementData->m_value != 0)
      {
         if (!m_animationActive)
         {
            StartAnimation(tableElementData);
         }
         else if (this->m_retriggerBehaviour == RetriggerBehaviourEnum::Restart)
         {
            StartAnimation(tableElementData);
         }
      }
      else
      {
         if (m_animationActive)
         {
            StopAnimation();
         }
      }
   }
}

template <typename MatrixElementType> void MatrixBitmapAnimationEffectBase<MatrixElementType>::StartAnimation(TableElementData* tableElementData)
{
   LoadAnimationFrames();

   if (m_animationFrames.empty())
   {

      MatrixBitmapEffectBase<MatrixElementType>::Trigger(tableElementData);
      return;
   }

   m_animationTableElementData = tableElementData;
   m_animationActive = true;
   m_currentFrame = 0;
   m_currentStep = 0;

   RenderCurrentFrame(tableElementData);
   AnimationStep();
}

template <typename MatrixElementType> void MatrixBitmapAnimationEffectBase<MatrixElementType>::StopAnimation()
{
   m_animationActive = false;
   m_animationTableElementData = nullptr;


   MatrixElementType inactiveValue = this->GetInactiveValue();
   for (int x = this->m_areaLeft; x <= this->m_areaRight; x++)
   {
      for (int y = this->m_areaTop; y <= this->m_areaBottom; y++)
      {
         this->m_matrix->SetElement(this->GetLayerNr(), x, y, inactiveValue);
      }
   }
}

template <typename MatrixElementType> void MatrixBitmapAnimationEffectBase<MatrixElementType>::AnimationStep()
{
   if (!m_animationActive || m_animationTableElementData == nullptr)
      return;


   m_currentStep++;


   int frameIndex = CalculateFrameIndex();


   bool shouldStop = false;
   if (m_animationBehaviour == AnimationBehaviourEnum::Once)
   {
      if (m_currentStep >= m_animationStepCount)
         shouldStop = true;
   }

   if (shouldStop)
   {
      StopAnimation();
      return;
   }


   if (frameIndex != m_currentFrame)
   {
      m_currentFrame = frameIndex;
      RenderCurrentFrame(m_animationTableElementData);
   }


   this->m_table->GetPinball()->GetAlarms()->RegisterAlarm(m_animationFrameDurationMs, [this]() { this->AnimationStep(); }, false);
}

template <typename MatrixElementType> void MatrixBitmapAnimationEffectBase<MatrixElementType>::RenderCurrentFrame(TableElementData* tableElementData)
{
   if (m_animationFrames.empty() || m_currentFrame >= static_cast<int>(m_animationFrames.size()))
      return;


   FastBitmap originalBitmap = this->m_bitmap;
   this->m_bitmap = m_animationFrames[m_currentFrame];
   this->m_bitmapLoaded = true;


   this->RenderBitmap(tableElementData);


   this->m_bitmap = originalBitmap;
}

template <typename MatrixElementType> void MatrixBitmapAnimationEffectBase<MatrixElementType>::LoadAnimationFrames()
{
   if (!m_animationFrames.empty())
      return;

   if (this->m_bitmapFilePattern.empty())
      return;


   for (int i = 0; i < m_animationStepCount; i++)
   {
      std::string filename = this->m_bitmapFilePattern;
      if (filename.find("{0}") != std::string::npos)
      {
         filename = StringExtensions::Replace(filename, "{0}", std::to_string(i));
      }

      FastBitmap frame(filename);
      if (frame.IsValid())
      {
         m_animationFrames.push_back(frame);
      }
      else
      {

         break;
      }
   }

   if (m_animationFrames.empty())
   {
      Log::Warning(StringExtensions::Build("Could not load any animation frames for pattern: {0}", this->m_bitmapFilePattern));
   }
}

template <typename MatrixElementType> int MatrixBitmapAnimationEffectBase<MatrixElementType>::CalculateFrameIndex() const
{
   if (m_animationFrames.empty())
      return 0;

   int frameCount = static_cast<int>(m_animationFrames.size());

   switch (m_animationStepDirection)
   {
   case MatrixAnimationStepDirectionEnum::Frame:

      if (m_animationBehaviour == AnimationBehaviourEnum::Loop)
      {
         return (m_currentStep / m_animationStepSize) % frameCount;
      }
      else
      {
         return std::min(m_currentStep / m_animationStepSize, frameCount - 1);
      }

   case MatrixAnimationStepDirectionEnum::FrameBackward:

      if (m_animationBehaviour == AnimationBehaviourEnum::Loop)
      {
         return frameCount - 1 - ((m_currentStep / m_animationStepSize) % frameCount);
      }
      else
      {
         return std::max(frameCount - 1 - (m_currentStep / m_animationStepSize), 0);
      }

   default: return 0;
   }
}

template <typename MatrixElementType> void MatrixBitmapAnimationEffectBase<MatrixElementType>::Finish()
{
   try
   {
   }
   catch (...)
   {
   }
   m_animationActive = false;
   m_animationTableElementData = nullptr;
   MatrixBitmapEffectBase<MatrixElementType>::Finish();
}

}