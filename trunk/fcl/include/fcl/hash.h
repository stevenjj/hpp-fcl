/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/** \author Jia Pan */

#ifndef FCL_HASH_H
#define FCL_HASH_H

#include <stdexcept>
#include <set>
#include <vector>
#include <list>
#include <boost/unordered_map.hpp>

namespace fcl
{

// HashFnc is any extended hash function: HashFnc(key) = {index1, index2, ..., }
template<typename Key, typename Data, typename HashFnc>
class SimpleHashTable
{
protected:
  typedef std::list<Data> Bin;
  std::vector<Bin> table_;
  HashFnc h_;
  size_t table_size_;
public:
  SimpleHashTable(const HashFnc& h) : h_(h)
  {
  }

  void init(size_t size)
  {
    if(size == 0) 
    {
      throw std::logic_error("SimpleHashTable must have non-zero size.");
    }

    table_.resize(size);
    table_size_ = size;
  }

  void insert(Key key, Data value)
  {
    std::vector<unsigned int> indices = h_(key);
    size_t range = table_.size();
    for(size_t i = 0; i < indices.size(); ++i)
      table_[indices[i] % range].push_back(value);
  }

  std::vector<Data> query(Key key) const
  {
    size_t range = table_.size();
    std::vector<unsigned int> indices = h_(key);
    std::set<Data> result;
    for(size_t i = 0; i < indices.size(); ++i)
    {
      unsigned int index = indices[i] % range;
      std::copy(table_[index].begin(), table_[index].end(), std::inserter(result, result.end()));
    }

    return std::vector<Data>(result.begin(), result.end());
  }

  void remove(Key key, Data value) 
  {
    size_t range = table_.size();
    std::vector<unsigned int> indices = h_(key);
    for(size_t i = 0; i < indices.size(); ++i)
    {
      unsigned int index = indices[i] % range;
      table_[index].remove(value);
    }
  }

  void clear() 
  {
    table_.clear();
    table_.resize(table_size_);
  }
};


template<typename U, typename V>
class unordered_map_hash_table : public boost::unordered_map<U, V> {};


template<typename Key, typename Data, typename HashFnc, template<typename, typename> class TableT = unordered_map_hash_table>
class SparseHashTable
{
protected:
  HashFnc h_;
  typedef std::list<Data> Bin;
  typedef TableT<size_t, Bin> Table;
  
  Table table_;
public:
  SparseHashTable(const HashFnc& h) : h_(h) {}

  void init(size_t) { table_.clear(); }
  
  void insert(Key key, Data value)
  {
    std::vector<unsigned int> indices = h_(key);
    for(size_t i = 0; i < indices.size(); ++i)
      table_[indices[i]].push_back(value);
  }

  std::vector<Data> query(Key key) const
  {
    std::vector<unsigned int> indices = h_(key);
    std::set<Data> result;
    for(size_t i = 0; i < indices.size(); ++i)
    {
      unsigned int index = indices[i];
      typename Table::const_iterator p = table_.find(index);
      if(p != table_.end())
        std::copy((*p).second.begin(), (*p).second.end(), std::inserter(result, result.end()));
    }

    return std::vector<Data>(result.begin(), result.end());
  }

  void remove(Key key, Data value)
  {
    std::vector<unsigned int> indices = h_(key);
    for(size_t i = 0; i < indices.size(); ++i)
    {
      unsigned int index = indices[i];
      table_[index].remove(value);
    }
  }

  void clear()
  {
    table_.clear();
  }
};


}

#endif