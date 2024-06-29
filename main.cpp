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
  int blockAmount = 12000;
  // число создаваемых массивов для тестирования
  int arrayAmount = 350;
  if(argc > 1)
    blockAmount = atoi (argv [1]);
  if(argc > 2)
    arrayAmount = atoi (argv [2]);

  sortedElementsModel model;
  for(int i=0; i < arrayAmount; ++i)
  {
    model.m_sortItemsQueue.push_back (new std::vector<std::string*>);

    // Задаем размер массива случайным образом
    int index = rand() % blockAmount;
    for(int j = 0; j < index; ++j)
    {
      model.m_sortItemsQueue.at(i)->push_back(new std::string(getRandomString()));
    }
  }
  std::cout <<"start sorting" << std::endl;
  model.sort();
  std::cout <<"end sorting" << std::endl;
  if (model.checkIsSorted ())
    std::cout << "sort completed success" << std::endl;

  std::cout << "enter 1 to ptint all elements" << std::endl;
  int N = 0;
  std::cin >> N;
  if(N ==1)
    model.printSortElements();
  return 1;
}
