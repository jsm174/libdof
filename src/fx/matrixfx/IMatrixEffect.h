#pragma once

#include "../IEffect.h"
#include "../FadeModeEnum.h"
#include <string>

namespace DOF
{

class IMatrixEffect : public virtual IEffect
{
public:
   virtual ~IMatrixEffect() = default;

   virtual FadeModeEnum GetFadeMode() const = 0;
   virtual void SetFadeMode(FadeModeEnum value) = 0;
   virtual float GetHeight() const = 0;
   virtual void SetHeight(float value) = 0;
   virtual int GetLayerNr() const = 0;
   virtual void SetLayerNr(int value) = 0;
   virtual float GetLeft() const = 0;
   virtual void SetLeft(float value) = 0;
   virtual float GetTop() const = 0;
   virtual void SetTop(float value) = 0;
   virtual const std::string& GetToyName() const = 0;
   virtual void SetToyName(const std::string& value) = 0;
   virtual float GetWidth() const = 0;
   virtual void SetWidth(float value) = 0;
};

}