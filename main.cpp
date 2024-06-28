#include <iostream>
#include "sortedElements.h"

std::string getRandomString()
{
  const std::string possibleSymbols = "ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyz1234567890";
  const int randomStrlength = 12;
    
  std::string randomStr;
  randomStr.resize(randomStrlength);

 // srand(time(NULL));

  for(int i=0; i < randomStrlength; ++i)
  {
    int index = rand() % possibleSymbols.length();
    randomStr[i] = possibleSymbols[index];
  }
  return randomStr;
}

int main(int argc, char const *argv[])
{
  int blockAmount = 100;
  if(argc > 1)
  {
    blockAmount = atoi (argv [1]);
  } 

  sortedElementsModel model;
  for(int i=0; i < 50; ++i)
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
  int testN;
  std::cin >> testN;
  std::cout << testN;
  //model.printSortElements();
  return 1;
}