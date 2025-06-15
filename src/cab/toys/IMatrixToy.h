#pragma once

#include "IToy.h"

namespace DOF
{

template <typename MatrixElementType> class IMatrixToy : public virtual IToy
{
public:
   virtual ~IMatrixToy() { }

   virtual MatrixElementType* GetLayer(int layerNr) = 0;
   virtual int GetHeight() const = 0;
   virtual int GetWidth() const = 0;
   virtual int GetElementCount() const { return GetWidth() * GetHeight(); }
   virtual MatrixElementType GetElement(int layerNr, int x, int y) = 0;
   virtual void SetElement(int layerNr, int x, int y, const MatrixElementType& value) = 0;
};

}