#include "timsort.h"

standardItem::standardItem(): m_data("")
{
}
standardItem::standardItem(std::string value):m_data(value)
{}
std::string standardItem::getData() const
{
    return m_data;
}