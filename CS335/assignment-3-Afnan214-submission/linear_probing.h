#ifndef LINEAR_PROBING_H
#define LINEAR_PROBING_H

#include <vector>
#include <algorithm>
#include <functional>

namespace {

// Internal method to test if a positive number is prime.
bool linear_IsPrime(size_t n) {
  if( n == 2 || n == 3 )
    return true;
  
  if( n == 1 || n % 2 == 0 )
    return false;
  
  for( int i = 3; i * i <= n; i += 2 )
    if( n % i == 0 )
      return false;
  
  return true;
}


// Internal method to return a prime number at least as large as n.
int linear_NextPrime(size_t n) {
  if (n % 2 == 0)
    ++n;  
  while (!linear_IsPrime(n)) n += 2;  
  return n;
}

}  // namespace

// linear probing implementation.
template <typename HashedObj>
class HashTableLinear {
 public:
  enum EntryType {ACTIVE, EMPTY, DELETED};

  explicit HashTableLinear(size_t size = 101) : array_(linear_NextPrime(size))
    { MakeEmpty(); }
  
  bool Contains(const HashedObj & x) const  {
    int probes =0;    // variable to pass as the second parameter to FindPos
    return IsActive(FindPos(x, probes));
  }
  
  void MakeEmpty() {
    current_size_ = 0;
    for (auto &entry : array_)
      entry.info_ = EMPTY;
  }

  bool Insert(const HashedObj & x) {
    // Insert x as active
    int probes=0;     // variable to pass as the second parameter to FindPos
    size_t current_pos = FindPos(x, probes);
    //num_collisions is added to. Will return the overall number of collisions experienced
    num_collisions += (probes - 1);
    if (IsActive(current_pos))
      return false;
    
    array_[current_pos].element_ = x;
    array_[current_pos].info_ = ACTIVE;
    num_elements++;
    // Rehash; see Section 5.5
    if (++current_size_ > array_.size()/2)
      Rehash();    
    return true;
  }
    
  bool Insert(HashedObj && x) {
    // Insert x as active
    int probes=0;     // variable to pass as the second parameter to FindPos
    size_t current_pos = FindPos(x, probes);
    //num_collisions is added to. Will return the overall number of collisions experienced
    num_collisions += (probes - 1);
    if (IsActive(current_pos))
      return false;
    
    array_[current_pos] = std::move(x);
    array_[current_pos].info_ = ACTIVE;
    
    // Rehash; see Section 5.5
    if (++current_size_ > array_.size()/2)
      Rehash();

    return true;
  }

  bool Remove(const HashedObj & x) {
    int probes = 0;      // variable to pass as the second parameter to FindPos
    size_t current_pos = FindPos(x, probes);
    num_collisions += (probes - 1);
    if (!IsActive(current_pos))
      return false;

    array_[current_pos].info_ = DELETED;
    num_elements--;
    return true;
  }
  
  //getter for the number of elements in the table.
  size_t get_num_elem()
  {
    return num_elements;
  }
  //getter for table size
  size_t get_table_size()
  {
    return array_.size();
  }
  //Getter for the number of collisions
  size_t get_num_collisions()
  {
    return num_collisions;
  }
  //returns the number of probes taken to find x
  int get_probes(const HashedObj & x, int& probes)
  {
    FindPos(x, probes); 
    return probes;
  }
 private:        
  struct HashEntry {
    HashedObj element_;
    EntryType info_;
    
    HashEntry(const HashedObj& e = HashedObj{}, EntryType i = EMPTY)
    :element_{e}, info_{i} { }
    
    HashEntry(HashedObj && e, EntryType i = EMPTY)
    :element_{std::move(e)}, info_{ i } {}
  };
    

  std::vector<HashEntry> array_;
  size_t num_elements=0;      // should tell the number of elements stored in the table
  size_t current_size_;
  size_t num_collisions =0;    // keep track of the number of collisions. added to by Insert and Remove
  bool IsActive(size_t current_pos) const
  { return array_[current_pos].info_ == ACTIVE; }

 size_t FindPos(const HashedObj & x, int& probes) const //added second arg probes to keep track of the number of probes when finding position for or of x
  {

    size_t current_pos = InternalHash(x);
    while (array_[current_pos].info_ != EMPTY && array_[current_pos].element_ != x) 
    {
      current_pos ++;  // since this is linear probing... increment by 1
      probes++;
      if (current_pos >= array_.size())
	      current_pos -= array_.size();
    }
    probes++;
    return current_pos;
  }

  void Rehash() 
  {
    std::vector<HashEntry> old_array = array_;

    // Create new double-sized, empty table.
    array_.resize(linear_NextPrime(2 * old_array.size()));
    for (auto & entry : array_)
      entry.info_ = EMPTY;
    
    // Copy table over.
    current_size_ = 0;
    for (auto & entry :old_array)
      if (entry.info_ == ACTIVE)
	  Insert(std::move(entry.element_));
  }
  
  size_t InternalHash(const HashedObj & x) const {
    static std::hash<HashedObj> hf;
    return hf(x) % array_.size( );
  }
};

#endif  // LINEAR_PROBING_H
