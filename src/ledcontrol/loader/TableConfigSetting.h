#pragma once

#include <string>
#include "OutputControlEnum.h"
#include "OutputTypeEnum.h"
#include "../../fx/matrixfx/MatrixShiftDirectionEnum.h"
#include "../../fx/matrixfx/MatrixAnimationStepDirectionEnum.h"
#include "../../fx/matrixfx/AnimationBehaviourEnum.h"

#include "DOF/DOF.h"

namespace DOF
{

class ColorConfig;

class TableConfigSetting
{
public:
   TableConfigSetting();
   TableConfigSetting(const std::string& settingData);
   ~TableConfigSetting();


   OutputControlEnum GetOutputControl() const { return m_outputControl; }
   void SetOutputControl(OutputControlEnum control) { m_outputControl = control; }

   const std::string& GetColorName() const { return m_colorName; }
   void SetColorName(const std::string& colorName) { m_colorName = colorName; }

   ColorConfig* GetColorConfig() const { return m_colorConfig; }
   void SetColorConfig(ColorConfig* config) { m_colorConfig = config; }

   const std::string& GetTableElement() const { return m_tableElement; }
   void SetTableElement(const std::string& element) { m_tableElement = element; }

   const std::string& GetCondition() const { return m_condition; }
   void SetCondition(const std::string& condition) { m_condition = condition; }

   OutputTypeEnum GetOutputType() const;
   void SetOutputType(OutputTypeEnum type) { m_outputType = type; }

   int GetDurationMs() const { return m_durationMs; }
   void SetDurationMs(int duration) { m_durationMs = duration; }

   int GetIntensity() const;
   void SetIntensity(int intensity) { m_intensity = intensity; }

   int GetMinDurationMs() const { return m_minDurationMs; }
   void SetMinDurationMs(int duration) { m_minDurationMs = duration; }

   int GetMaxDurationMs() const { return m_maxDurationMs; }
   void SetMaxDurationMs(int duration) { m_maxDurationMs = duration; }

   int GetExtDurationMs() const { return m_extDurationMs; }
   void SetExtDurationMs(int duration) { m_extDurationMs = duration; }

   int GetFadingUpDurationMs() const { return m_fadingUpDurationMs; }
   void SetFadingUpDurationMs(int duration) { m_fadingUpDurationMs = duration; }

   int GetFadingDownDurationMs() const { return m_fadingDownDurationMs; }
   void SetFadingDownDurationMs(int duration) { m_fadingDownDurationMs = duration; }

   int GetBlink() const { return m_blink; }
   void SetBlink(int blink) { m_blink = blink; }

   int GetBlinkIntervalMs() const { return m_blinkIntervalMs; }
   void SetBlinkIntervalMs(int interval) { m_blinkIntervalMs = interval; }

   int GetBlinkIntervalMsNested() const { return m_blinkIntervalMsNested; }
   void SetBlinkIntervalMsNested(int interval) { m_blinkIntervalMsNested = interval; }

   int GetBlinkPulseWidth() const { return m_blinkPulseWidth; }
   void SetBlinkPulseWidth(int width);

   int GetBlinkPulseWidthNested() const { return m_blinkPulseWidthNested; }
   void SetBlinkPulseWidthNested(int width);

   bool GetInvert() const { return m_invert; }
   void SetInvert(bool invert) { m_invert = invert; }

   bool GetNoBool() const { return m_noBool; }
   void SetNoBool(bool noBool) { m_noBool = noBool; }

   int GetWaitDurationMs() const { return m_waitDurationMs; }
   void SetWaitDurationMs(int duration) { m_waitDurationMs = duration; }

   int GetLayer() const { return m_layer; }
   void SetLayer(int layer) { m_layer = layer; }
   bool HasLayer() const { return m_layer != -1; }

   bool IsArea() const { return m_isArea; }
   void SetIsArea(bool isArea) { m_isArea = isArea; }


   int GetAreaLeft() const { return m_areaLeft; }
   void SetAreaLeft(int left) { m_areaLeft = left; }

   int GetAreaTop() const { return m_areaTop; }
   void SetAreaTop(int top) { m_areaTop = top; }

   int GetAreaWidth() const { return m_areaWidth; }
   void SetAreaWidth(int width) { m_areaWidth = width; }

   int GetAreaHeight() const { return m_areaHeight; }
   void SetAreaHeight(int height) { m_areaHeight = height; }

   int GetAreaSpeed() const { return m_areaSpeed; }
   void SetAreaSpeed(int speed) { m_areaSpeed = speed; }

   int GetAreaAcceleration() const { return m_areaAcceleration; }
   void SetAreaAcceleration(int acceleration) { m_areaAcceleration = acceleration; }

   int GetAreaFlickerDensity() const { return m_areaFlickerDensity; }
   void SetAreaFlickerDensity(int density) { m_areaFlickerDensity = density; }

   int GetAreaFlickerMinDurationMs() const { return m_areaFlickerMinDurationMs; }
   void SetAreaFlickerMinDurationMs(int duration) { m_areaFlickerMinDurationMs = duration; }

   int GetAreaFlickerMaxDurationMs() const { return m_areaFlickerMaxDurationMs; }
   void SetAreaFlickerMaxDurationMs(int duration) { m_areaFlickerMaxDurationMs = duration; }

   int GetAreaFlickerFadeDurationMs() const { return m_areaFlickerFadeDurationMs; }
   void SetAreaFlickerFadeDurationMs(int duration) { m_areaFlickerFadeDurationMs = duration; }

   MatrixShiftDirectionEnum GetAreaDirection() const { return m_areaDirection; }
   void SetAreaDirection(MatrixShiftDirectionEnum direction)
   {
      m_areaDirection = direction;
      m_hasAreaDirection = true;
   }
   bool HasAreaDirection() const { return m_hasAreaDirection; }
   void ClearAreaDirection() { m_hasAreaDirection = false; }


   bool IsBitmap() const { return m_isBitmap; }
   void SetIsBitmap(bool isBitmap) { m_isBitmap = isBitmap; }

   int GetAreaBitmapTop() const { return m_areaBitmapTop; }
   void SetAreaBitmapTop(int top) { m_areaBitmapTop = top; }

   int GetAreaBitmapLeft() const { return m_areaBitmapLeft; }
   void SetAreaBitmapLeft(int left) { m_areaBitmapLeft = left; }

   int GetAreaBitmapWidth() const { return m_areaBitmapWidth; }
   void SetAreaBitmapWidth(int width) { m_areaBitmapWidth = width; }

   int GetAreaBitmapHeight() const { return m_areaBitmapHeight; }
   void SetAreaBitmapHeight(int height) { m_areaBitmapHeight = height; }

   int GetAreaBitmapFrame() const { return m_areaBitmapFrame; }
   void SetAreaBitmapFrame(int frame) { m_areaBitmapFrame = frame; }

   int GetAreaBitmapAnimationStepSize() const { return m_areaBitmapAnimationStepSize; }
   void SetAreaBitmapAnimationStepSize(int size) { m_areaBitmapAnimationStepSize = size; }

   int GetAreaBitmapAnimationStepCount() const { return m_areaBitmapAnimationStepCount; }
   void SetAreaBitmapAnimationStepCount(int count) { m_areaBitmapAnimationStepCount = count; }

   int GetAreaBitmapAnimationFrameDuration() const { return m_areaBitmapAnimationFrameDuration; }
   void SetAreaBitmapAnimationFrameDuration(int duration) { m_areaBitmapAnimationFrameDuration = duration; }

   MatrixAnimationStepDirectionEnum GetAreaBitmapAnimationDirection() const { return m_areaBitmapAnimationDirection; }
   void SetAreaBitmapAnimationDirection(MatrixAnimationStepDirectionEnum direction) { m_areaBitmapAnimationDirection = direction; }

   AnimationBehaviourEnum GetAreaBitmapAnimationBehaviour() const { return m_areaBitmapAnimationBehaviour; }
   void SetAreaBitmapAnimationBehaviour(AnimationBehaviourEnum behaviour) { m_areaBitmapAnimationBehaviour = behaviour; }


   bool IsPlasma() const { return m_isPlasma; }
   void SetIsPlasma(bool isPlasma) { m_isPlasma = isPlasma; }

   int GetPlasmaSpeed() const { return m_plasmaSpeed; }
   void SetPlasmaSpeed(int speed) { m_plasmaSpeed = speed; }

   int GetPlasmaDensity() const { return m_plasmaDensity; }
   void SetPlasmaDensity(int density) { m_plasmaDensity = density; }

   const std::string& GetColorName2() const { return m_colorName2; }
   void SetColorName2(const std::string& colorName) { m_colorName2 = colorName; }

   ColorConfig* GetColorConfig2() const { return m_colorConfig2; }
   void SetColorConfig2(ColorConfig* config) { m_colorConfig2 = config; }


   const std::string& GetShapeName() const { return m_shapeName; }
   void SetShapeName(const std::string& shape) { m_shapeName = shape; }


   int GetBlinkLow() const { return m_blinkLow; }
   void SetBlinkLow(int low) { m_blinkLow = low; }


   void ParseSettingData(const std::string& settingData);

private:
   OutputControlEnum m_outputControl;
   std::string m_colorName;
   ColorConfig* m_colorConfig;
   std::string m_tableElement;
   std::string m_condition;
   OutputTypeEnum m_outputType;
   int m_durationMs;
   int m_minDurationMs;
   int m_maxDurationMs;
   int m_extDurationMs;
   int m_intensity;
   int m_fadingUpDurationMs;
   int m_fadingDownDurationMs;
   int m_blink;
   int m_blinkIntervalMs;
   int m_blinkIntervalMsNested;
   int m_blinkPulseWidth;
   int m_blinkPulseWidthNested;
   int m_blinkLow;
   bool m_invert;
   bool m_noBool;
   int m_waitDurationMs;
   int m_layer;
   bool m_isArea;


   int m_areaLeft;
   int m_areaTop;
   int m_areaWidth;
   int m_areaHeight;
   int m_areaSpeed;
   int m_areaAcceleration;
   int m_areaFlickerDensity;
   int m_areaFlickerMinDurationMs;
   int m_areaFlickerMaxDurationMs;
   int m_areaFlickerFadeDurationMs;
   MatrixShiftDirectionEnum m_areaDirection;
   bool m_hasAreaDirection;


   bool m_isBitmap;
   int m_areaBitmapTop;
   int m_areaBitmapLeft;
   int m_areaBitmapWidth;
   int m_areaBitmapHeight;
   int m_areaBitmapFrame;
   int m_areaBitmapAnimationStepSize;
   int m_areaBitmapAnimationStepCount;
   int m_areaBitmapAnimationFrameDuration;
   MatrixAnimationStepDirectionEnum m_areaBitmapAnimationDirection;
   AnimationBehaviourEnum m_areaBitmapAnimationBehaviour;


   bool m_isPlasma;
   int m_plasmaSpeed;
   int m_plasmaDensity;
   std::string m_colorName2;
   ColorConfig* m_colorConfig2;


   std::string m_shapeName;
};

}