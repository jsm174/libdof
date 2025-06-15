#include "ShapeAnimated.h"
#include "../../../general/MathExtensions.h"

namespace DOF
{

ShapeAnimated::ShapeAnimated()
   : m_animationStepDirection(MatrixAnimationStepDirectionEnum::Frame)
   , m_animationStepSize(1)
   , m_animationFrameCount(1)
   , m_animationBehaviour(AnimationBehaviourEnum::Loop)
   , m_animationFrameDurationMs(30)
{
}

ShapeAnimated::~ShapeAnimated() { }

MatrixAnimationStepDirectionEnum ShapeAnimated::GetAnimationStepDirection() const { return m_animationStepDirection; }

void ShapeAnimated::SetAnimationStepDirection(MatrixAnimationStepDirectionEnum value) { m_animationStepDirection = value; }

int ShapeAnimated::GetAnimationStepSize() const { return m_animationStepSize; }

void ShapeAnimated::SetAnimationStepSize(int value) { m_animationStepSize = value; }

int ShapeAnimated::GetAnimationFrameCount() const { return m_animationFrameCount; }

void ShapeAnimated::SetAnimationFrameCount(int value) { m_animationFrameCount = MathExtensions::Limit(value, 1, INT_MAX); }

AnimationBehaviourEnum ShapeAnimated::GetAnimationBehaviour() const { return m_animationBehaviour; }

void ShapeAnimated::SetAnimationBehaviour(AnimationBehaviourEnum value) { m_animationBehaviour = value; }

int ShapeAnimated::GetAnimationFrameDurationMs() const { return m_animationFrameDurationMs; }

void ShapeAnimated::SetAnimationFrameDurationMs(int value) { m_animationFrameDurationMs = MathExtensions::Limit(value, 1, INT_MAX); }

}