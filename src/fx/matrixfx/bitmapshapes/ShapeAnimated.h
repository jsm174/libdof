#pragma once

#include "Shape.h"
#include "../MatrixAnimationStepDirectionEnum.h"
#include "../AnimationBehaviourEnum.h"

namespace DOF
{

class ShapeAnimated : public Shape
{
private:
   MatrixAnimationStepDirectionEnum m_animationStepDirection;
   int m_animationStepSize;
   int m_animationFrameCount;
   AnimationBehaviourEnum m_animationBehaviour;
   int m_animationFrameDurationMs;

public:
   ShapeAnimated();
   virtual ~ShapeAnimated();

   MatrixAnimationStepDirectionEnum GetAnimationStepDirection() const;
   void SetAnimationStepDirection(MatrixAnimationStepDirectionEnum value);

   int GetAnimationStepSize() const;
   void SetAnimationStepSize(int value);

   int GetAnimationFrameCount() const;
   void SetAnimationFrameCount(int value);

   AnimationBehaviourEnum GetAnimationBehaviour() const;
   void SetAnimationBehaviour(AnimationBehaviourEnum value);

   int GetAnimationFrameDurationMs() const;
   void SetAnimationFrameDurationMs(int value);
};

}