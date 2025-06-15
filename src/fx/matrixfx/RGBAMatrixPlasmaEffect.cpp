#include "RGBAMatrixPlasmaEffect.h"
#include "../../general/MathExtensions.h"

namespace DOF
{

RGBAMatrixPlasmaEffect::RGBAMatrixPlasmaEffect()
   : m_activeColor(0xff, 0x00, 0x00, 0xff)
   , m_inactiveColor(0, 0, 0, 0)
   , m_secondaryColor(0x00, 0x00, 0xff, 0xff)
{
}

RGBAColor RGBAMatrixPlasmaEffect::GetInactiveValue() { return m_inactiveColor; }

RGBAColor RGBAMatrixPlasmaEffect::GetPlasmaValue(int triggerValue, double intensity)
{
   RGBAColor d;

   int v = MathExtensions::Limit(triggerValue, 0, 255);

   double primaryWeight = intensity;
   double secondaryWeight = 1.0 - intensity;

   int mixedRed = (int)(m_activeColor.GetRed() * primaryWeight + m_secondaryColor.GetRed() * secondaryWeight);
   int mixedGreen = (int)(m_activeColor.GetGreen() * primaryWeight + m_secondaryColor.GetGreen() * secondaryWeight);
   int mixedBlue = (int)(m_activeColor.GetBlue() * primaryWeight + m_secondaryColor.GetBlue() * secondaryWeight);
   int mixedAlpha = (int)(m_activeColor.GetAlpha() * primaryWeight + m_secondaryColor.GetAlpha() * secondaryWeight);

   d.SetRed(m_inactiveColor.GetRed() + MathExtensions::Limit((int)((float)(mixedRed - m_inactiveColor.GetRed()) * v / 255), 0, 255));
   d.SetGreen(m_inactiveColor.GetGreen() + MathExtensions::Limit((int)((float)(mixedGreen - m_inactiveColor.GetGreen()) * v / 255), 0, 255));
   d.SetBlue(m_inactiveColor.GetBlue() + MathExtensions::Limit((int)((float)(mixedBlue - m_inactiveColor.GetBlue()) * v / 255), 0, 255));
   d.SetAlpha(m_inactiveColor.GetAlpha() + MathExtensions::Limit((int)((float)(mixedAlpha - m_inactiveColor.GetAlpha()) * v / 255), 0, 255));

   return d;
}

}