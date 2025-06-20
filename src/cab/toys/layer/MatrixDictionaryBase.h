#pragma once

#include <map>
#include <algorithm>
#include "../../../general/MathExtensions.h"

namespace DOF
{

template <typename MatrixElementType> class MatrixDictionaryBase : public std::map<int, MatrixElementType*>
{
public:
   MatrixDictionaryBase();
   MatrixDictionaryBase(int width, int height);
   ~MatrixDictionaryBase();

   MatrixElementType* GetLayer(int layerNr);
   MatrixElementType* GetOrCreateLayer(int layerNr);
   void SetLayer(int layerNr, MatrixElementType* data);
   int GetWidth() const { return m_width; }
   void SetWidth(int value);
   int GetHeight() const { return m_height; }
   void SetHeight(int value);
   int GetElementCount() const { return m_width * m_height; }
   MatrixElementType GetElement(int layerNr, int x, int y);
   void SetElement(int layerNr, int x, int y, const MatrixElementType& value);
   void Clear();

private:
   int m_width;
   int m_height;

   MatrixElementType* CreateLayer();
   int GetIndex(int x, int y) const { return y * m_width + x; }
};

template <typename MatrixElementType>
MatrixDictionaryBase<MatrixElementType>::MatrixDictionaryBase()
   : m_width(1)
   , m_height(1)
{
}

template <typename MatrixElementType>
MatrixDictionaryBase<MatrixElementType>::MatrixDictionaryBase(int width, int height)
   : m_width(MathExtensions::Limit(width, 1, INT_MAX))
   , m_height(MathExtensions::Limit(height, 1, INT_MAX))
{
}

template <typename MatrixElementType> MatrixDictionaryBase<MatrixElementType>::~MatrixDictionaryBase() { Clear(); }

template <typename MatrixElementType> MatrixElementType* MatrixDictionaryBase<MatrixElementType>::GetLayer(int layerNr)
{
   auto it = this->find(layerNr);
   if (it != this->end())
   {
      return it->second;
   }
   return nullptr;
}

template <typename MatrixElementType> MatrixElementType* MatrixDictionaryBase<MatrixElementType>::GetOrCreateLayer(int layerNr)
{
   auto it = this->find(layerNr);
   if (it != this->end())
   {
      return it->second;
   }


   MatrixElementType* newLayer = CreateLayer();
   (*this)[layerNr] = newLayer;
   return newLayer;
}

template <typename MatrixElementType> void MatrixDictionaryBase<MatrixElementType>::SetLayer(int layerNr, MatrixElementType* data)
{

   auto it = this->find(layerNr);
   if (it != this->end())
   {
      delete[] it->second;
      this->erase(it);
   }

   if (data != nullptr)
      (*this)[layerNr] = data;
}

template <typename MatrixElementType> void MatrixDictionaryBase<MatrixElementType>::SetWidth(int value)
{
   int newWidth = MathExtensions::Limit(value, 1, INT_MAX);
   if (newWidth != m_width)
   {

      Clear();
      m_width = newWidth;
   }
}

template <typename MatrixElementType> void MatrixDictionaryBase<MatrixElementType>::SetHeight(int value)
{
   int newHeight = MathExtensions::Limit(value, 1, INT_MAX);
   if (newHeight != m_height)
   {

      Clear();
      m_height = newHeight;
   }
}

template <typename MatrixElementType> MatrixElementType MatrixDictionaryBase<MatrixElementType>::GetElement(int layerNr, int x, int y)
{
   if (x < 0 || x >= m_width || y < 0 || y >= m_height)
      return MatrixElementType();

   MatrixElementType* layer = GetLayer(layerNr);
   if (layer != nullptr)
      return layer[GetIndex(x, y)];
   return MatrixElementType();
}

template <typename MatrixElementType> void MatrixDictionaryBase<MatrixElementType>::SetElement(int layerNr, int x, int y, const MatrixElementType& value)
{
   if (x < 0 || x >= m_width || y < 0 || y >= m_height)
      return;

   MatrixElementType* layer = GetOrCreateLayer(layerNr);
   if (layer != nullptr)
      layer[GetIndex(x, y)] = value;
}

template <typename MatrixElementType> void MatrixDictionaryBase<MatrixElementType>::Clear()
{
   for (auto& pair : *this)
      delete[] pair.second;
   std::map<int, MatrixElementType*>::clear();
}

template <typename MatrixElementType> MatrixElementType* MatrixDictionaryBase<MatrixElementType>::CreateLayer()
{
   int elementCount = GetElementCount();
   MatrixElementType* layer = new MatrixElementType[elementCount];


   for (int i = 0; i < elementCount; i++)
      layer[i] = MatrixElementType();

   return layer;
}


}