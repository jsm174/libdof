#pragma once

#include <unordered_map>
#include <vector>

namespace DOF
{

template <typename LayerElementType> class LayerDictionary
{
public:
   LayerDictionary();
   LayerDictionary(int width, int height);
   ~LayerDictionary();

   LayerElementType* GetLayer(int layerNr, int expectedSize);
   LayerElementType* GetOrCreateLayer(int layerNr, int expectedSize);
   std::vector<int> GetLayerNumbers() const;
   bool HasLayer(int layerNr) const;
   void RemoveLayer(int layerNr);
   void Clear();
   size_t GetLayerCount() const { return m_layers.size(); }
   int GetWidth() const { return m_width; }
   int GetHeight() const { return m_height; }
   void SetDimensions(int width, int height);

private:
   std::unordered_map<int, LayerElementType*> m_layers;
   int m_width;
   int m_height;

   LayerElementType* CreateLayer(int expectedSize);
};

template <typename LayerElementType>
LayerDictionary<LayerElementType>::LayerDictionary()
   : m_width(1)
   , m_height(1)
{
}

template <typename LayerElementType>
LayerDictionary<LayerElementType>::LayerDictionary(int width, int height)
   : m_width(width)
   , m_height(height)
{
}

template <typename LayerElementType> LayerDictionary<LayerElementType>::~LayerDictionary() { Clear(); }

template <typename LayerElementType> LayerElementType* LayerDictionary<LayerElementType>::GetLayer(int layerNr, int expectedSize)
{
   auto it = m_layers.find(layerNr);
   if (it != m_layers.end())
   {
      return it->second;
   }
   return nullptr;
}

template <typename LayerElementType> LayerElementType* LayerDictionary<LayerElementType>::GetOrCreateLayer(int layerNr, int expectedSize)
{
   auto it = m_layers.find(layerNr);
   if (it != m_layers.end())
   {
      return it->second;
   }

   LayerElementType* newLayer = CreateLayer(expectedSize);
   m_layers[layerNr] = newLayer;
   return newLayer;
}

template <typename LayerElementType> std::vector<int> LayerDictionary<LayerElementType>::GetLayerNumbers() const
{
   std::vector<int> layerNumbers;
   for (const auto& pair : m_layers)
   {
      layerNumbers.push_back(pair.first);
   }
   return layerNumbers;
}

template <typename LayerElementType> bool LayerDictionary<LayerElementType>::HasLayer(int layerNr) const { return m_layers.find(layerNr) != m_layers.end(); }

template <typename LayerElementType> void LayerDictionary<LayerElementType>::RemoveLayer(int layerNr)
{
   auto it = m_layers.find(layerNr);
   if (it != m_layers.end())
   {
      delete[] it->second;
      m_layers.erase(it);
   }
}

template <typename LayerElementType> void LayerDictionary<LayerElementType>::Clear()
{
   for (auto& pair : m_layers)
   {
      delete[] pair.second;
   }
   m_layers.clear();
}

template <typename LayerElementType> void LayerDictionary<LayerElementType>::SetDimensions(int width, int height)
{
   m_width = width;
   m_height = height;
}

template <typename LayerElementType> LayerElementType* LayerDictionary<LayerElementType>::CreateLayer(int expectedSize)
{
   LayerElementType* layer = new LayerElementType[expectedSize];


   for (int i = 0; i < expectedSize; i++)
      layer[i] = LayerElementType();

   return layer;
}

}