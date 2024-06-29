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
    bool addItemsVector(std::vector<std::vector<std::string* >*> items);
    /**
     * @brief sort - Сортировать массивы
     * @param order - Порядок сортировки (по возрастанию или по убыванию)
     * @return
     */
    bool sort(sortOrder order = sortOrder::ascending);
    /**
     * @brief printSortElements - Напечатать отсортированные элементы
     */
    void printSortElements();

    /**
    * @brief printSortElements - Проверить отсортированы ли массивы
    */
    bool checkIsSorted (sortOrder order = sortOrder::ascending);

    std::vector<std::vector<std::string* >* > m_sortItemsQueue;
};
