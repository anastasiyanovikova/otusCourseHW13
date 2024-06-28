#include "sortedElements.h"
#include <thread>
#include <algorithm>

void sortItems (std::vector < standardItem* >& items, sortedElementsModel::sortOrder order)
{
     auto funcLess = [](standardItem* a, standardItem* b){
        return a < b;
    };

    auto funcGrater = [](standardItem* a, standardItem* b){
        return b < a;
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
        auto compare = order == sortedElementsModel::ascending ? funcLess : funcGrater;
        std::stable_sort(items.begin (), items.end (), compare);
        std::cout << "sort end"<<std::endl;
    }  
}
void sortItemsVector (sortedElementsModel::sortOrder order, std::vector < std::vector < standardItem* >* >  &vector_items)
{
    for (auto v: vector_items)
    sortItems (*v, order);
}
standardItem::standardItem(): m_data("")
{
}
standardItem::standardItem(std::string value):m_data(value)
{}
std::string standardItem::getData() const
{
    return m_data;
}
/*
bool standardItem::operator<(const standardItem& other) const
{
    return m_data < other.getData();
}
bool standardItem::operator<=(const standardItem& other) const
{
    return m_data <= other.getData();
}
bool standardItem::operator==(const standardItem& other) const
{
    return m_data == other.getData();   
}

*/
sortedElementsModel::sortedElementsModel(){}

bool sortedElementsModel::addItemsVector(std::vector<standardItem*> items)
{
    if(items.size() > 0)
        m_sortItemsQueue.push_back(items);
    return true;
}

//Сортировка маленьких массивов отдельно

bool sortedElementsModel::sort(sortOrder order)
{
  static const uint item_amount_for_one_thread = 1000;

  auto calc_thread_amount = [] (const uint items_amount,
                                const uint     item_amount_for_one_thread,
                                const uint     sort_items_amount,
                                const bool     small_vector_of_items_is_empty, 
                                const bool     small_vector_of_items_is_big)
  {
    static unsigned int ideal_thread_amount = std::thread::hardware_concurrency();
    std::cout << ideal_thread_amount<< " - idral thread" << std::endl;
    uint res;
    if (small_vector_of_items_is_big)
      res = ideal_thread_amount;
    else
    {
      res = std::min (std::min (ideal_thread_amount, sort_items_amount), static_cast <uint> (items_amount / item_amount_for_one_thread));
      if (!small_vector_of_items_is_empty  &&  res > 1)
        res -= 1;
    }
    
    return res;
  };

  auto item_amount_comparator = [] (const std::pair < int, std::vector < std::vector < standardItem* >* > >& v1,
                                    const std::pair < int, std::vector < std::vector < standardItem* >* > >& v2)
  {
    return v1.first < v2.first;
  };


  if (!m_sortItemsQueue.empty())
  
    {
    std::vector < std::vector < standardItem* >* > sort_items_paral;
    std::vector < std::vector < standardItem* >* > small_vectors_of_items;
    uint32_t items_amount = 0;
    uint reserve_size = 0;
    for (auto sq_it : m_sortItemsQueue)
    {
    reserve_size += sq_it.size();
    }
    sort_items_paral.reserve (reserve_size);
    small_vectors_of_items.reserve (reserve_size);

    static const int small_mass_size = 300;
    for (auto sq_it : m_sortItemsQueue)
    {
        if(sq_it.size() <= small_mass_size)
            small_vectors_of_items.push_back(&sq_it);
        else
        {
            sort_items_paral.push_back(&sq_it);
            items_amount += sq_it.size();
        }
    }
    
    const bool small_vector_of_items_is_big = (small_vectors_of_items.size () >= 1000) ? true : false;

    const uint8_t number_of_additional_threads = calc_thread_amount (items_amount, item_amount_for_one_thread, sort_items_paral.size (), small_vectors_of_items.empty (), small_vector_of_items_is_big);

    if ((items_amount < item_amount_for_one_thread  &&  small_vectors_of_items.empty () )  ||  (!number_of_additional_threads && small_vectors_of_items.empty ()))
    {
        sortItemsVector (order, sort_items_paral);
        for(auto vec: sort_items_paral)
        {
            std::cout <<"**&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&**************"<<std::endl;
                for(auto itemr: *vec)
                    std::cout << itemr->getData()<<std::endl;
        }
    }
    else if (items_amount <= item_amount_for_one_thread && !small_vector_of_items_is_big)
    {
    sortItemsVector (order, sort_items_paral);
    sortItemsVector (order, small_vectors_of_items);

        for(auto vec: sort_items_paral)
        {
            std::cout <<"+++++++++++++++++++++++++++++++++++++**************"<<std::endl;
                for(auto itemr: *vec)
                    std::cout << itemr->getData()<<std::endl;
        }
        std:: cout << small_vectors_of_items.size()<< "small vator" << std::endl;
        for(size_t i = 0; i <  small_vectors_of_items.size(); ++i)
        {
            if(nullptr == small_vectors_of_items.at(i))
                continue;
            std::cout << small_vectors_of_items.at(i)->size()<<std::endl;
            std::cout <<"**&-----------------------------------**************"<<std::endl;
            for(size_t itemr = 0; itemr < small_vectors_of_items.at(i)->size(); ++itemr)
                std::cout << small_vectors_of_items.at(i)->at(itemr)->getData()<<std::endl;
        }
    }
    else
    {
    const uint32_t one_thread_size = items_amount / number_of_additional_threads;
    
    std::vector < std::pair < int, std::vector < std::vector < standardItem* >* > > > items_in_one_thread;
    items_in_one_thread.reserve (number_of_additional_threads);
    
    size_t current_thread = 0;
    size_t current_item = 0;

    if (sort_items_paral.empty ())
    {
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
        auto it = std::min_element (items_in_one_thread.begin (),
                                    items_in_one_thread.end (),
                                    item_amount_comparator);

        it->first += sort_items_paral.at (current_item)->size ();
        it->second.push_back (sort_items_paral.at (current_item));
        }

        if (small_vector_of_items_is_big)
        {
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

    std::cout << "before sort" << items_in_one_thread.size() << std::endl;

    const int last = (small_vectors_of_items.empty () || small_vector_of_items_is_big) ? items_in_one_thread.size () - 1 : items_in_one_thread.size ();
    
    std::vector<std::thread> theads;
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
    
    std::cout << items_in_one_thread.size() << std::endl;
    for(auto vec: items_in_one_thread)
    {
        for(auto res: vec.second)
        {
            std::cout <<"**&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&**************"<<std::endl;
            for(auto itemr: *res)
                std::cout << itemr->getData()<<std::endl;
        }
    }
    std::cout << small_vectors_of_items.size() << std::endl;

    for(auto vec: small_vectors_of_items)
    {
        std::cout <<"**&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&**************"<<std::endl;
            for(auto itemr: *vec)
                std::cout << itemr->getData()<<std::endl;
    }
    }
    return true;
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
        for(auto item: vec)
        {
            std::cout << item->getData()<<std::endl;
        }
    }
}