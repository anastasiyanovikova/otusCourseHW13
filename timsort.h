#pragma once 

#include<iostream>
#include<vector>
#include<algorithm>
#include<string>
#include <math.h>

//typedef int SORT_TYPE;
//typedef std::string SORT_TYPE;
class standardItem
{
public:
    standardItem();
    standardItem(std::string value);

    std::string getData() const;

    //bool operator<(const standardItem& other) const;
    //bool operator<=(const standardItem& other) const;
    //bool operator==(const standardItem& other) const;
private:
    std::string m_data;
};

inline bool operator<(const standardItem& a, const standardItem&b)
{
    return a.getData() < b.getData();
}

typedef standardItem* SORT_TYPE;

struct segment {
  int first_index_in_segment; // индекс первого элемента
  int segment_length; // длина интервала
  segment() {}
  segment(int b, int l) : first_index_in_segment(b), segment_length(l) {}
};
// ответ должен быть в диапазоне (32,64]
inline int get_min_run(int n) {
  int r = 0;
  while (n >= 64) {
    n >>= 1;
    r |= n & 1;
  }
  return n + r;
}

/* Function used to do a binary search for binary insertion sort */
inline size_t  BinaryInsertionFind(std::vector<SORT_TYPE> &dst, const SORT_TYPE x, const size_t begin,
  const size_t size) {
  size_t l, c, r;
  SORT_TYPE cx;
  l = 0;
  r = size - 1;
  c = r >> 1;
  if(c)
  {

  }
  /* check for out of bounds at the beginning. */
  //if (SORT_CMP(x, dst[begin]) < 0) {
  //if (x < dst.at(begin)) {
  if (*x < (*dst.at(begin))) {
    return begin;
  }
 // else if (x > dst.at(r)) {
  else if (*(dst.at(r))<*(x)) {
    return r;
  }

  size_t size1 = size - begin;
  size_t r1 = size1 - 1;
  size_t c1 = r1 >> 1;

  cx = dst.at(begin + c1);

  while (1) {
    // const int val = SORT_CMP(x, cx);
    const int val = *(x) < *(cx) ? -1 : *(cx) < *(x) ? 1 : 0;

    if (val < 0) {
      if (c1 - l <= 1) {
        return c1 + begin;
      }

      r1 = c1;
    }
    else { /* allow = for stability. The binary search favors the right. */
      if (r1 - c1 <= 1) {
        return c1 + 1 + begin;
      }

      l = c1;
    }

    c1 = l + ((r1 - l) >> 1);
    cx = dst.at(begin + c1);
  }
}

inline void BinaryInsertionSortStart(std::vector<SORT_TYPE> &dst, const size_t start, const size_t size) {
  size_t i;

  for (i = start + 1; i < size; i++) {
    size_t j;
    SORT_TYPE x;
    size_t location;

      if(*dst.at(i-1) < (*dst.at(i))){
      continue;
    }

    /* Else we need to find the right place, shift everything over, and squeeze in */
    x = dst.at(i);
    location = BinaryInsertionFind(dst, x, start, i);

    for (j = i - 1; j >= location; j--) {
      dst.at(j + 1) = dst.at(j);

      if (j == 0) { /* check edge case because j is unsigned */
        break;
      }
    }

    dst.at(location) = x;
  }
}

inline void galloptmp(std::vector<SORT_TYPE> &mas, std::vector<SORT_TYPE> &tmp, int &posF, int &posS, int &pos, int lastF, int lastS)
{
  if(lastS < 0)
    std::cout << "gallop";
  int i_gollop = 0;
  int i = posF + std::pow(2, i_gollop);
  int i_prev = posF;
  bool f = true;
  if (i >= lastF)
    f = false;
  while (f == true && *(tmp.at(i)) < *(mas.at(posS)))
  {
    i_prev = i;
    ++i_gollop;
    //if (i + pow(2, i_gollop) < lastF-1)
    if (i + std::pow(2, i_gollop) < lastF)
      i = i + std::pow(2, i_gollop);
    else
      f = false;
  }
  copy(tmp.begin() + posF, tmp.begin() + i_prev, mas.begin() + pos + 1);
  pos = pos + i_prev - posF;
  posF = i_prev;
}

inline void gallopmas(std::vector<SORT_TYPE> &mas, std::vector<SORT_TYPE> &tmp, int &posF, int &posS, int &pos, int lastF, int lastS)
{
  if(lastF < 0)
    std::cout << "gallop";
  int i_gollop = 0;
  int i = posS + std::pow(2, i_gollop);
  int i_prev = posS;
  bool f = true;
  if (i >= lastS)
    f = false;
  while (f == true && *(mas.at(i)) < *(tmp.at(posF)))
  {
    i_prev = i;
    ++i_gollop;
    if (i + std::pow(2, i_gollop) < lastS)
      i = i + std::pow(2, i_gollop);
    else
      f = false;
  }
  copy(mas.begin() + posS, mas.begin() + i_prev, mas.begin() + pos + 1);
  pos = pos + i_prev - posS;
  posS = i_prev;
}

inline void merge(std::vector<SORT_TYPE> &mas, std::vector<segment> &seg, int seg_begin)
{
  int firstSeg = seg_begin;
  int secondSeg = seg_begin + 1;
  int posF = 0;
  int posS = seg.at(secondSeg).first_index_in_segment;
  int lastF = seg.at(firstSeg).segment_length;
  int lastS = seg.at(secondSeg).first_index_in_segment + seg.at(secondSeg).segment_length;
  int pos = seg.at(firstSeg).first_index_in_segment - 1;
  std::vector<SORT_TYPE> tmp(seg.at(firstSeg).segment_length);
  copy(mas.begin() + seg.at(firstSeg).first_index_in_segment, mas.begin() + seg.at(firstSeg).first_index_in_segment + seg.at(firstSeg).segment_length, tmp.begin());
  int fAmount = 0;
  int sAmount = 0;
  while (posF < lastF && posS < lastS)
  {
    if (*(tmp.at(posF)) < *(mas.at(posS)))
    {
      mas.at(++pos) = tmp.at(posF++);
      ++fAmount;
      sAmount = 0;
      if (fAmount == 7)
      {
        galloptmp(mas, tmp, posF, posS, pos, lastF, lastS);
      }
    }
    else
    {
      mas.at(++pos) = mas.at(posS++);
      fAmount = 0;
      ++sAmount;
      if (sAmount == 7)
      {
        gallopmas(mas, tmp, posF, posS, pos, lastF, lastS);
      }
    }

  }
  copy(tmp.begin() + posF, tmp.begin() + lastF, mas.begin() + pos + 1);
}
inline void try_merge(std::vector<SORT_TYPE> &mas, std::vector<segment> &seg)
{
  if (seg.size() == 2)
  {
    merge(mas, seg, 0);
    seg.at(0).segment_length = seg.at(0).segment_length + seg.at(1).segment_length;
    seg.pop_back();
  }
  else
  {
    int x = seg.at(seg.size() - 1).segment_length;
    int y = seg.at(seg.size() - 2).segment_length;
    int z = seg.at(seg.size() - 3).segment_length;
    if (x <= y + z || y <= z)
    {
      if (x < z)
      {
        merge(mas, seg, seg.size() - 2);
        seg.at(seg.size() - 2).segment_length += seg.at(seg.size() - 1).segment_length;
        seg.pop_back();
      }
      else
      {
        merge(mas, seg, seg.size() - 3);
        seg.at(seg.size() - 3).segment_length += seg.at(seg.size() - 2).segment_length;
        seg.at(seg.size() - 2) = seg.at(seg.size() - 1);
        seg.pop_back();
      }
    }

  }
}
inline int findSort(std::vector<SORT_TYPE> &mas, int first)
{
  int begin = first;
  bool p = false, n = false, s = true;
  if ( (size_t)(first + 1) == mas.size())
    return 1;
  if (*(mas.at(first)) < *(mas.at(first + 1)))
  {
    p = true;
  }
  else
    n = true;
  while ((size_t)(first + 1) < mas.size() && s)
  {
    if (*(mas.at(first)) < *(mas.at(first + 1)) && p == true)
      first++;
    else
      if (*(mas.at(first + 1)) < *(mas.at(first))  && n == true)
        first++;
      else
        s = false;
  }
  if (n == true)
  {
    int end = first;
    while (begin < end)
    {
      std::swap(mas.at(begin), mas.at(end));
      begin++;
      end--;
    }
  }
  return first;
}


inline void TimSort(std::vector<SORT_TYPE> &mas)
{
  int array_size = mas.size();
  if (array_size < 64)
  {
    BinaryInsertionSortStart(mas, 0, array_size);
  }
  else
  {
    int min_run = get_min_run(array_size);
    int size = min_run;
    std::vector<segment> seg;
    seg.reserve((array_size - 1) / min_run + 1);
    int i = 0;
    int lastEl = 0;
    while (i < array_size)
    {
      int l = 0;
      l = findSort(mas, lastEl) - lastEl;
      if (l < min_run)
      {
        size = std::min(min_run, array_size - i);
        seg.push_back(segment(i, size));
        lastEl = i + size;
        i += min_run;

      }
      else
      {
        seg.push_back(segment(i, l));
        lastEl = i + l;
        i += l;
      }
    }

    for (size_t i = 0; i < seg.size(); ++i)
    {
      BinaryInsertionSortStart(mas, seg.at(i).first_index_in_segment, seg.at(i).first_index_in_segment + seg.at(i).segment_length);
    }
    std::vector<segment> seg_merge;
    seg_merge.push_back(seg.at(0));
    seg_merge.push_back(seg.at(1));
    int seg_size_merge = 2;
    while (seg_merge.size() != 1) {
      if ( size_t(seg_size_merge) < seg.size())
      {
        seg_merge.push_back(seg.at(seg_size_merge));
        seg_size_merge++;
      }
      try_merge(mas, seg_merge);
    }
  }
}