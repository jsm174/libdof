#include "Shape.h"

namespace DOF
{

Shape::Shape()
   : m_bitmapFrameNumber(0)
   , m_bitmapTop(0)
   , m_bitmapLeft(0)
   , m_bitmapWidth(-1)
   , m_bitmapHeight(-1)
   , m_dataExtractMode(FastBitmapDataExtractModeEnum::BlendPixels)
{
}

Shape::~Shape() { }

int Shape::GetBitmapFrameNumber() const { return m_bitmapFrameNumber; }

void Shape::SetBitmapFrameNumber(int value) { m_bitmapFrameNumber = value; }

int Shape::GetBitmapTop() const { return m_bitmapTop; }

void Shape::SetBitmapTop(int value) { m_bitmapTop = value; }

int Shape::GetBitmapLeft() const { return m_bitmapLeft; }

void Shape::SetBitmapLeft(int value) { m_bitmapLeft = value; }

int Shape::GetBitmapWidth() const { return m_bitmapWidth; }

void Shape::SetBitmapWidth(int value) { m_bitmapWidth = value; }

int Shape::GetBitmapHeight() const { return m_bitmapHeight; }

void Shape::SetBitmapHeight(int value) { m_bitmapHeight = value; }

FastBitmapDataExtractModeEnum Shape::GetDataExtractMode() const { return m_dataExtractMode; }

void Shape::SetDataExtractMode(FastBitmapDataExtractModeEnum value) { m_dataExtractMode = value; }

}