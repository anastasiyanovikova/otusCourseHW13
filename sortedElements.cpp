#include "sortedElements.h"
#include <thread>
#include <algorithm>

/**
 * @brief sortItems - Сортировка набора массивов
 * @param items - Набор массивов
 * @param order - По возрастанию/ по убыванию
 */
void sortItems (std::vector < std::string * >& items, sortedElementsModel::sortOrder order)
{
     auto funcLess = [](std::string* a, std::string* b){
        return *a < *b;
    };

    auto funcGrater = [](std::string *a, std::string* b){
        return *b < *a;
    };

    if(items.size() > 10000)
    {
        TimSort(items);
        if(order == sortedElementsModel::descending)
        {
            std::reverse(items.begin (), items.end ());
        }
    }
    else
    {
        //auto compare = order == sortedElementsModel::ascending ? funcLess : funcGrater;
        //std::stable_sort (items.begin (), items.end (), compare);
        if(order == sortedElementsModel::ascending)
          std::stable_sort(items.begin (), items.end (), funcLess);
        else
          std::stable_sort (items.begin (), items.end (), funcGrater);
    }  
}
/**
 * @brief sortItemsVector - Сортировка одного массива строк
 * @param order - По возрастанию, по убыванию
 * @param vector_items - Массив для сортировки
 */
void sortItemsVector (sortedElementsModel::sortOrder order, std::vector < std::vector < std::string* >* >  &vector_items)
{
    for (auto v: vector_items)
        sortItems (*v, order);
}

sortedElementsModel::sortedElementsModel(){}

bool sortedElementsModel::addItemsVector(std::vector<std::vector<std::string* >*> items)
{
    //if(items.size() > 0)
  m_sortItemsQueue = items; // push_back(&items);
    return true;
}

bool sortedElementsModel::sort(sortOrder order)
{
    /// количество элементов в одном потоке, необходимое для создания потока
    static const size_t item_amount_for_one_thread = 1000;
    /// Функция определения числа дополнительных потоков
    auto calc_thread_amount = [] (const size_t items_amount,
                                const size_t     item_amount_for_one_thread,
                                const size_t     sort_items_amount,
                                const bool     small_vector_of_items_is_empty, 
                                const bool     small_vector_of_items_is_big)
    {
        static size_t ideal_thread_amount = std::thread::hardware_concurrency();
        std::cout << " ideal_thread_amount = " << ideal_thread_amount << std::endl;
        size_t res;
        if (small_vector_of_items_is_big)
          res = ideal_thread_amount;
        else
        {
          res = std::min (std::min (ideal_thread_amount, sort_items_amount), static_cast <size_t> (items_amount / item_amount_for_one_thread));
          if (!small_vector_of_items_is_empty  &&  res > 1)
            res -= 1;
        }

        return res;
    };

      auto item_amount_comparator = [] (const std::pair < int, std::vector < std::vector < std::string* >* > >& v1,
                                        const std::pair < int, std::vector < std::vector < std::string*>* > >& v2)
      {
        return v1.first < v2.first;
      };


    if (!m_sortItemsQueue.empty())
    {
        /// массивы, которые могут сортироваться параллельно - количество элементов в массиве больше 300
        std::vector < std::vector < std::string* > * > sort_items_paral;
        /// небольшие массивы, которые будут сортиооваться в одном потоке
        std::vector < std::vector < std::string* >* > small_vectors_of_items;
        uint32_t items_amount = 0;
        
        /// определим общее число элементов в массивах
        size_t reserve_size = m_sortItemsQueue.size ();
        sort_items_paral.reserve (reserve_size);
        small_vectors_of_items.reserve (reserve_size);

        /// делим массивы в зависимости от размера
        static const int small_mass_size = 300;
        for (auto sq_it : m_sortItemsQueue)
        {
            if(sq_it->size() <= small_mass_size)
                small_vectors_of_items.push_back(sq_it);
            else
            {
                sort_items_paral.push_back(sq_it);
                items_amount += sq_it->size();
            }
        }

        const bool small_vector_of_items_is_big = (small_vectors_of_items.size () >= 1000) ? true : false;
        const uint8_t number_of_additional_threads = calc_thread_amount (items_amount, item_amount_for_one_thread, sort_items_paral.size (), small_vectors_of_items.empty (), small_vector_of_items_is_big);

        /// если количество элементов меньше количества, начиная с которого имеет смысл создавать потоки, или необходим только один поток, то сортируем в главном потоке
        if ((items_amount < item_amount_for_one_thread  &&  small_vectors_of_items.empty () )  ||  (!number_of_additional_threads && small_vectors_of_items.empty ()))
        {
            sortItemsVector (order, sort_items_paral);
        }
        ///  если количество элементов меньше количества, начиная с которого имеет смысл создавать потоки и есть только маленькие массивы,то сортируем последовательно
        else if (items_amount <= item_amount_for_one_thread && !small_vector_of_items_is_big)
        {
            sortItemsVector (order, sort_items_paral);
            sortItemsVector (order, small_vectors_of_items);
        }
        /// иначе будем создавать потоки для сортировки
        else
        {
            /// количество элементов для одного потока
            const uint32_t one_thread_size = items_amount / number_of_additional_threads;
            std::vector < std::pair < int, std::vector < std::vector < std::string *>*> > > items_in_one_thread;
            items_in_one_thread.reserve (number_of_additional_threads);

            size_t current_thread = 0;
            size_t current_item = 0;

            ///много маленьких массивов
            if (sort_items_paral.empty ())
            {
                /// формирование массивов для запуска в отдельных потоках
                items_in_one_thread.push_back ({static_cast <int> (small_vectors_of_items.at (current_item)->size ()), {small_vectors_of_items.at (current_item),}});
                ++current_item;

                while ( current_item < small_vectors_of_items.size () && current_thread < number_of_additional_threads)
                {
                if (current_item < small_vectors_of_items.size () && items_in_one_thread.at (current_thread).first + small_vectors_of_items.at (current_item)->size () < one_thread_size)
                {
                    items_in_one_thread.at (current_thread).first += small_vectors_of_items.at (current_item)->size ();
                    items_in_one_thread.at (current_thread).second.push_back (small_vectors_of_items.at (current_item));
                    ++current_item;
                }
                else
                {
                    ++current_thread;
                    if (current_item < small_vectors_of_items.size () && current_thread < number_of_additional_threads)
                    {
                    items_in_one_thread.push_back ({static_cast <int> (small_vectors_of_items.at (current_item)->size ()), {small_vectors_of_items.at (current_item),}});
                    ++current_item;
                    }
                    else
                    break;
                }
                }

                for (; current_item < small_vectors_of_items.size (); ++current_item)
                {
                auto it = std::min_element (items_in_one_thread.begin (),
                                            items_in_one_thread.end (),
                                            item_amount_comparator);

                it->first += small_vectors_of_items.at (current_item)->size ();
                it->second.push_back (small_vectors_of_items.at (current_item));
                }
            }
            else
            {
                items_in_one_thread.push_back ({static_cast <int> (sort_items_paral.at (current_item)->size ()), {sort_items_paral.at (current_item),}});
                ++current_item;

                while (current_item < sort_items_paral.size () && current_thread < number_of_additional_threads)
                {
                    if (current_item < sort_items_paral.size () && items_in_one_thread.at (current_thread).first + sort_items_paral.at (current_item)->size () < one_thread_size)
                    {
                        items_in_one_thread.at (current_thread).first += sort_items_paral.at (current_item)->size ();
                        items_in_one_thread.at (current_thread).second.push_back (sort_items_paral.at (current_item));
                        ++current_item;
                    }
                    else
                    {
                        ++current_thread;
                        if (current_item < sort_items_paral.size () && current_thread < number_of_additional_threads)
                        {
                        items_in_one_thread.push_back ({static_cast <int> (sort_items_paral.at (current_item)->size ()), {sort_items_paral.at (current_item),}});
                        ++current_item;
                        }
                        else
                        break;
                    }
                }

                for (; current_item < sort_items_paral.size (); ++current_item)
                {
                    /// добавление происходит в поток, в котором на данный момент мньше элементов
                    auto it = std::min_element (items_in_one_thread.begin (),
                                                items_in_one_thread.end (),
                                                item_amount_comparator);

                    it->first += sort_items_paral.at (current_item)->size ();
                    it->second.push_back (sort_items_paral.at (current_item));
                }

                if (small_vector_of_items_is_big)
                {
                    /// если много маленьких массивов, то их тоже разделим между потоками
                    current_item = 0;
                    if (items_in_one_thread.size () < number_of_additional_threads)
                    {
                        while (current_item < small_vectors_of_items.size () && current_thread < number_of_additional_threads)
                        {
                            if (current_item < small_vectors_of_items.size () && items_in_one_thread.at (current_thread).first + small_vectors_of_items.at (current_item)->size () < one_thread_size)
                            {
                                items_in_one_thread.at (current_thread).first += small_vectors_of_items.at (current_item)->size ();
                                items_in_one_thread.at (current_thread).second.push_back (small_vectors_of_items.at (current_item));
                                ++current_item;
                            }
                            else
                            {
                                ++current_thread;
                                if (current_item < small_vectors_of_items.size () && current_thread < number_of_additional_threads)
                                {
                                items_in_one_thread.push_back ({static_cast <int> (small_vectors_of_items.at (current_item)->size ()), {small_vectors_of_items.at (current_item),}});
                                ++current_item;
                                }
                                else
                                break;
                            }
                        }
                    }

                    for (; current_item < small_vectors_of_items.size (); ++current_item)
                    {
                        auto it = std::min_element (items_in_one_thread.begin (),
                                                items_in_one_thread.end (),
                                                item_amount_comparator);

                        it->first += small_vectors_of_items.at (current_item)->size ();
                        it->second.push_back (small_vectors_of_items.at (current_item));
                    }
                }
            }

            const int last = (small_vectors_of_items.empty () || small_vector_of_items_is_big) ? items_in_one_thread.size () - 1 : items_in_one_thread.size ();

            std::vector<std::thread> theads;
            /// создаем на 1 меньше потоко, чем надо, так как основной поток тоже будет использован для сортировки
            for (int i = 0; i < last; ++i)
            {
                theads.emplace_back([order, i, items_in_one_thread]() mutable
                    {
                        sortItemsVector(order, items_in_one_thread.at (i).second);
                    });
              //threads.emplace_back(std::thread(sortItemsVector, order, items_in_one_thread.at (i).second));
            }

            if (small_vectors_of_items.empty () || small_vector_of_items_is_big)
                sortItemsVector (order, items_in_one_thread.at (last).second);
            else
                sortItemsVector (order, small_vectors_of_items);

            for(auto& th: theads)
                th.join();
        }
    }
    return true;
}

void sortedElementsModel::printSortElements()
{
    std::cout << "print Elements" <<std::endl;
    int i = 0;
    for(auto vec: m_sortItemsQueue)
    {
        std::cout << i << " ***************************************************************"<<std::endl;
        ++i;
        for(auto item: *vec)
        {
            std::cout << *item <<std::endl;
        }
    }
}

bool sortedElementsModel::checkIsSorted (sortOrder order)
{
  if (order == sortOrder::ascending)
  {
    for (auto vec : m_sortItemsQueue)
    {
      for (size_t i = 0; i <  vec->size() - 1; i++)
      {
        if (*(vec->at (i)) > *(vec->at (i + 1)))
        {
          std::cout << "error in sorting" << std::endl;
          for (auto item : *vec)
          {
            std::cout << *item << std::endl;
          }
          return false;
        }
      }
    }
  }
  else
  {
    for (auto vec : m_sortItemsQueue)
    {
      for (size_t i = 0; i < vec->size () - 1; i++)
      {
        if (*(vec->at (i)) < *(vec->at (i + 1)))
        {
          std::cout << "error in sorting" << std::endl;

          for (auto item : *vec)
          {
            std::cout << *item << std::endl;
          }

          return false;
        }
      }
    }
  }
  return true;
}

