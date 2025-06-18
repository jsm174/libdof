#include "RGBAMatrixShapeEffect.h"
#include "bitmapshapes/ShapeAnimated.h"
#include "bitmapshapes/ShapeDefinitions.h"
#include "../../table/TableElementData.h"
#include "../../table/Table.h"
#include "../../general/MathExtensions.h"
#include <algorithm>

namespace DOF
{

RGBAMatrixShapeEffect::RGBAMatrixShapeEffect()
   : MatrixBitmapEffectBase<RGBAColor>()
   , m_shapeDefinition(nullptr)
   , m_shapeDefinitions(nullptr)
{
}

RGBAMatrixShapeEffect::~RGBAMatrixShapeEffect() { }

const std::string& RGBAMatrixShapeEffect::GetShapeName() const { return m_shapeName; }

void RGBAMatrixShapeEffect::SetShapeName(const std::string& value) { m_shapeName = value; }

void RGBAMatrixShapeEffect::Init(Table* table)
{
   MatrixBitmapEffectBase<RGBAColor>::Init(table);

   if (!table || !table->GetShapeDefinitions())
      return;

   Shape* shapeDef = nullptr;
   ShapeList& shapes = table->GetShapeDefinitions()->GetShapes();
   for (Shape* shape : shapes)
   {
      if (shape && shape->GetName() == m_shapeName)
      {
         shapeDef = shape;
         break;
      }
   }

   if (shapeDef != nullptr)
   {

      m_shapeDefinition = shapeDef;
      m_shapeDefinitions = table->GetShapeDefinitions();

      FilePattern* pattern = table->GetShapeDefinitions()->GetBitmapFilePattern();
      if (pattern)
         this->SetBitmapFilePattern(pattern->GetPattern());

      this->SetBitmapFrameNumber(shapeDef->GetBitmapFrameNumber());
      this->SetBitmapHeight(shapeDef->GetBitmapHeight());
      this->SetBitmapWidth(shapeDef->GetBitmapWidth());
      this->SetBitmapTop(shapeDef->GetBitmapTop());
      this->SetBitmapLeft(shapeDef->GetBitmapLeft());
   }
   else
   {

      m_shapeDefinition = nullptr;
      m_shapeDefinitions = nullptr;
   }
}

void RGBAMatrixShapeEffect::Finish()
{
   MatrixBitmapEffectBase<RGBAColor>::Finish();
   m_shapeDefinition = nullptr;
   m_shapeDefinitions = nullptr;
}

RGBAColor RGBAMatrixShapeEffect::GetInactiveValue() { return RGBAColor(0, 0, 0, 0); }

RGBAColor RGBAMatrixShapeEffect::GetPixelValue(const PixelData& pixel, int triggerValue)
{

   int v = MathExtensions::Limit(triggerValue, 0, 255);

   RGBAColor result;
   result.SetRed(MathExtensions::Limit((int)((float)pixel.red * v / 255), 0, 255));
   result.SetGreen(MathExtensions::Limit((int)((float)pixel.green * v / 255), 0, 255));
   result.SetBlue(MathExtensions::Limit((int)((float)pixel.blue * v / 255), 0, 255));
   result.SetAlpha(MathExtensions::Limit((int)((float)pixel.alpha * v / 255), 0, 255));

   return result;
}

}