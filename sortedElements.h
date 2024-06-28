#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include "timsort.h"

/**
 * @brief The sortedElementsModel class - Класс упрощенной модели для сортировки
 */
class sortedElementsModel{
public:
    enum sortOrder{
        ascending = 0,
        descending
    };
    sortedElementsModel();
    /**
     * @brief addItemsVector - Добавить массив для сортировки в очередь сортировки
     * @param items - Массив элементов
     * @return
     */
    bool addItemsVector(std::vector<standardItem*> items);
    /**
     * @brief sort - Сортировать массивы
     * @param order - Порядок сортировки (по возрастанию или по убыванию)
     * @return
     */
    bool sort(sortOrder order = sortOrder::ascending);
    /**
     * @brief printSortElements - Напечатать элементы
     */
    void printSortElements();
private:
    std::vector<std::vector<standardItem*>> m_sortItemsQueue;    
};
