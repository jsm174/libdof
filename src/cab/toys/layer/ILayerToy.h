#pragma once

#include "../IToy.h"
#include "LayerDictionary.h"

namespace DOF
{

template <typename LayerElementType> class ILayerToy : public virtual IToy
{
public:
   virtual ~ILayerToy() = default;

   virtual LayerDictionary<LayerElementType>& GetLayers() = 0;
   virtual const LayerDictionary<LayerElementType>& GetLayers() const = 0;
};

}