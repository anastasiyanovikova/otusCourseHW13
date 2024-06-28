#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include "timsort.h"

class sortedElementsModel{
public:
    enum sortOrder{
        ascending = 0,
        descending
    };
    sortedElementsModel();
    bool addItemsVector(std::vector<standardItem*> items);
    bool sort(sortOrder order = sortOrder::ascending);
    void printSortElements();
private:
    std::vector<std::vector<standardItem*>> m_sortItemsQueue;    
};