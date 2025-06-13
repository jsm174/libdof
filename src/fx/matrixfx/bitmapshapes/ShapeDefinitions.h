#pragma once

#include "ShapeList.h"
#include "../../../general/FilePattern.h"
#include <string>

namespace DOF
{

class ShapeDefinitions
{
private:
   FilePattern* m_bitmapFilePattern;
   ShapeList m_shapes;

public:
   ShapeDefinitions();
   virtual ~ShapeDefinitions();

   FilePattern* GetBitmapFilePattern() const;
   void SetBitmapFilePattern(FilePattern* value);

   ShapeList& GetShapes();
   void SetShapes(const ShapeList& value);

   std::string GetShapeDefinitionsXml();
   void SaveShapeDefinitionsXmlFile(const std::string& fileName);

   static ShapeDefinitions* GetShapeDefinitionsFromShapeDefinitionsXmlFile(const std::string& fileName);
   static bool TestShapeDefinitionsShapeDefinitionsXmlFile(const std::string& fileName);
   static ShapeDefinitions* GetShapeDefinitionsFromShapeDefinitionsXml(const std::string& shapeDefinitionsXml);
};

}