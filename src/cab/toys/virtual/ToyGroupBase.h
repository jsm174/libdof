#pragma once

#include "../ToyBaseUpdatable.h"
#include "../IMatrixToy.h"
#include "../layer/ILayerToy.h"
#include "../layer/LayerDictionary.h"
#include "../../Cabinet.h"
#include "../ToyList.h"
#include <vector>
#include <string>

namespace DOF
{

template <typename MatrixElementType> class ToyGroupBase : public ToyBaseUpdatable, public IMatrixToy<MatrixElementType>
{
public:
   ToyGroupBase();
   virtual ~ToyGroupBase();

   virtual void Init(Cabinet* cabinet) override;
   virtual void Reset() override;
   virtual void Finish() override;
   virtual void UpdateToy() override;
   virtual MatrixElementType* GetLayer(int layerNr) override;
   virtual int GetHeight() const override;
   virtual int GetWidth() const override;
   virtual MatrixElementType GetElement(int layerNr, int x, int y) override;
   virtual void SetElement(int layerNr, int x, int y, const MatrixElementType& value) override;
   const std::vector<std::vector<std::string>>& GetToyNames() const { return m_toyNames; }
   void SetToyNames(const std::vector<std::vector<std::string>>& toyNames) { m_toyNames = toyNames; }
   int GetLayerOffset() const { return m_layerOffset; }
   void SetLayerOffset(int offset) { m_layerOffset = offset; }
   LayerDictionary<MatrixElementType>& GetLayers() { return m_layers; }
   const LayerDictionary<MatrixElementType>& GetLayers() const { return m_layers; }

protected:
   virtual void UpdateOutputs() override;

private:
   std::vector<std::vector<std::string>> m_toyNames;
   std::vector<std::vector<ILayerToy<MatrixElementType>*>> m_toys;
   LayerDictionary<MatrixElementType> m_layers;
   int m_layerOffset;
   int m_width;
   int m_height;

   void InitializeToyMatrix(Cabinet* cabinet);
};

template <typename MatrixElementType>
ToyGroupBase<MatrixElementType>::ToyGroupBase()
   : m_layerOffset(0)
   , m_width(0)
   , m_height(0)
{
}

template <typename MatrixElementType> ToyGroupBase<MatrixElementType>::~ToyGroupBase() { }

template <typename MatrixElementType> void ToyGroupBase<MatrixElementType>::Init(Cabinet* cabinet)
{


   InitializeToyMatrix(cabinet);


   m_layers = LayerDictionary<MatrixElementType>(m_width, m_height);
}

template <typename MatrixElementType> void ToyGroupBase<MatrixElementType>::Reset() { m_layers = LayerDictionary<MatrixElementType>(m_width, m_height); }

template <typename MatrixElementType> void ToyGroupBase<MatrixElementType>::Finish() { m_toys.clear(); }

template <typename MatrixElementType> void ToyGroupBase<MatrixElementType>::UpdateToy() { UpdateOutputs(); }

template <typename MatrixElementType> void ToyGroupBase<MatrixElementType>::UpdateOutputs()
{

   std::vector<int> layerNumbers = m_layers.GetLayerNumbers();

   for (int layerNr : layerNumbers)
   {
      int targetLayerNr = layerNr + m_layerOffset;
      MatrixElementType* layerData = m_layers.GetLayer(layerNr, m_width * m_height);

      if (layerData != nullptr)
      {
         for (int y = 0; y < m_height; y++)
         {
            for (int x = 0; x < m_width; x++)
            {
               if (y < static_cast<int>(m_toys.size()) && x < static_cast<int>(m_toys[y].size()) && m_toys[y][x] != nullptr)
               {

                  LayerDictionary<MatrixElementType>& targetLayers = m_toys[y][x]->GetLayers();
                  MatrixElementType* targetLayer = targetLayers.GetOrCreateLayer(targetLayerNr, 1);
                  if (targetLayer != nullptr)
                  {
                     targetLayer[0] = layerData[y * m_width + x];
                  }
               }
            }
         }
      }
   }
}

template <typename MatrixElementType> MatrixElementType* ToyGroupBase<MatrixElementType>::GetLayer(int layerNr) { return m_layers.GetLayer(layerNr, m_width * m_height); }

template <typename MatrixElementType> int ToyGroupBase<MatrixElementType>::GetHeight() const { return m_height; }

template <typename MatrixElementType> int ToyGroupBase<MatrixElementType>::GetWidth() const { return m_width; }

template <typename MatrixElementType> MatrixElementType ToyGroupBase<MatrixElementType>::GetElement(int layerNr, int x, int y)
{
   MatrixElementType* layer = m_layers.GetLayer(layerNr, m_width * m_height);
   if (layer != nullptr && x >= 0 && x < m_width && y >= 0 && y < m_height)
   {
      return layer[y * m_width + x];
   }
   return MatrixElementType();
}

template <typename MatrixElementType> void ToyGroupBase<MatrixElementType>::SetElement(int layerNr, int x, int y, const MatrixElementType& value)
{
   MatrixElementType* layer = m_layers.GetOrCreateLayer(layerNr, m_width * m_height);
   if (layer != nullptr && x >= 0 && x < m_width && y >= 0 && y < m_height)
   {
      layer[y * m_width + x] = value;
   }
}

template <typename MatrixElementType> void ToyGroupBase<MatrixElementType>::InitializeToyMatrix(Cabinet* cabinet)
{
   if (cabinet == nullptr)
      return;


   m_height = static_cast<int>(m_toyNames.size());
   m_width = 0;

   if (m_height > 0)
   {

      for (const auto& row : m_toyNames)
      {
         m_width = std::max(m_width, static_cast<int>(row.size()));
      }
   }

   if (m_height == 0 || m_width == 0)
   {

      m_height = 1;
      m_width = 1;
      m_toyNames = { { "" } };
   }


   m_toys.clear();
   m_toys.resize(m_height);

   for (int y = 0; y < m_height; y++)
   {
      m_toys[y].resize(m_width, nullptr);

      for (int x = 0; x < m_width; x++)
      {
         if (y < static_cast<int>(m_toyNames.size()) && x < static_cast<int>(m_toyNames[y].size()) && !m_toyNames[y][x].empty())
         {

            IToy* toy = cabinet->GetToys()->FindByName(m_toyNames[y][x]);
            ILayerToy<MatrixElementType>* layerToy = dynamic_cast<ILayerToy<MatrixElementType>*>(toy);
            m_toys[y][x] = layerToy;
         }
      }
   }
}

}