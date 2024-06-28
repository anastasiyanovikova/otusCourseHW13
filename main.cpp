#include <iostream>
#include "sortedElements.h"

/**
 * @brief getRandomString - Функция генерации случайной строки для тестирования
 * @return случайная строка
 */
std::string getRandomString()
{
  const std::string possibleSymbols = "ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyz1234567890";
  const int randomStrlength = 12;
    
  std::string randomStr;
  randomStr.resize(randomStrlength);
  for(int i=0; i < randomStrlength; ++i)
  {
    int index = rand() % possibleSymbols.length();
    randomStr[i] = possibleSymbols[index];
  }
  return randomStr;
}

int main(int argc, char const *argv[])
{
  // максимальное число элементов в массиве можно задать при запуске программы
  int blockAmount = 1100;
  // число создаваемых массивов для тестирования
  int arrayAmount = 350;
  if(argc > 1)
    blockAmount = atoi (argv [1]);
  if(argc > 2)
    arrayAmount = atoi (argv [2]);

  sortedElementsModel model;
  for(int i=0; i < arrayAmount; ++i)
  {
    std::vector<standardItem*> items;

    int index = rand() % blockAmount;
    for(int j = 0; j < index; ++j)
    {
      items.push_back(new standardItem(getRandomString()));
    }
    model.addItemsVector(items);
  }
  std::cout <<"start sorting" << std::endl;
  model.sort();
  std::cout <<"end sorting" << std::endl;

  //model.printSortElements();
  return 1;
}
