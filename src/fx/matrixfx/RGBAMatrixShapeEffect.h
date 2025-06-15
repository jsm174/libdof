#pragma once

#include "MatrixBitmapEffectBase.h"
#include "../../general/color/RGBAColor.h"
#include "bitmapshapes/Shape.h"
#include "bitmapshapes/ShapeDefinitions.h"
#include <string>

namespace DOF
{

class TableElementData;
class Table;

class RGBAMatrixShapeEffect : public MatrixBitmapEffectBase<RGBAColor>
{
private:
   std::string m_shapeName;
   Shape* m_shapeDefinition;
   ShapeDefinitions* m_shapeDefinitions;

protected:
   virtual RGBAColor GetInactiveValue() override;
   virtual RGBAColor GetPixelValue(const PixelData& pixel, int triggerValue) override;

public:
   RGBAMatrixShapeEffect();
   virtual ~RGBAMatrixShapeEffect();

   const std::string& GetShapeName() const;
   void SetShapeName(const std::string& value);

   virtual void Init(Table* table) override;
   virtual void Finish() override;


   virtual std::string GetXmlElementName() const override { return "RGBAMatrixShapeEffect"; }
};

}