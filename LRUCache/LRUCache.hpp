#include <unordered_map>

typedef int key_t;
typedef int val_t;

struct PositionNode{
  key_t key_;
  PositionNode* older_;
  PositionNode* newer_;

  PositionNode(const key_t& key)
    : key_(key)
    , older_(nullptr)
    , newer_(nullptr) {
  }
  
  key_t get_key() const { return key_; }  
  void set_key(const key_t newkey) {key_= newkey; }  
};

struct LoadValue{
  val_t val_;     
  PositionNode* pos_node_;

  LoadValue(const val_t& val, PositionNode* pos)
    :val_(val)
    , pos_node_(pos) {
  }
  
  void set_value(const val_t& val) { val_ = val; }
  void set_pos_node(PositionNode* pos_node) { pos_node_ = pos_node; }
  val_t get_value() const { return val_; }
  PositionNode* get_pos_node() const {return pos_node_; }
};

class PositionList{
public:
  PositionList(): newest_(nullptr), oldest_(nullptr) {}
  // to delete all the nodes
  ~PositionList();
  
  // add a new key to the list, first position: newest
  PositionNode* add_new(const key_t& key);
  // move the node to the fist position
  void update_to_newest(PositionNode* node);
  // get the oldest node
  PositionNode* get_oldest() const { return oldest_; }
  // get the newest node
  PositionNode* get_newest() const { return newest_; }
  // set the node to become the newest
  void set_newest(PositionNode* node);
  
  // testing purposes
  // return the newest key
  key_t get_newest_key() const { return newest_->get_key(); }
  // return the oldest key, no position updates
  key_t get_oldest_key() const { return oldest_->get_key(); }

private:
  PositionNode* newest_;
  PositionNode* oldest_;
};

// useful type definitions
typedef std::unordered_map<key_t, LoadValue*> cache_map_t;
typedef cache_map_t::size_type capacity_t;

class CacheTable{
public:
  CacheTable(const capacity_t& _cap): capacity_(_cap) {}
  // to delete all the LoadValues
  ~CacheTable();

  // insert a new key,val to the cache
  void insert(const key_t& key, const val_t& val);
  // get the value for a given key
  val_t get_value(const key_t& key);

  // testing purposes
  // return newest key
  key_t get_newest_key() const { return pos_list_.get_newest_key(); }
  // return oldest key, no position updates
  key_t get_oldest_key() const { return pos_list_.get_oldest_key(); }
  
private:
  capacity_t capacity_;
  cache_map_t map_;
  PositionList pos_list_;
};

// Just a wrapper class to export and test
class LRUCache{
public:
  LRUCache(const capacity_t& cap): cache_(CacheTable(cap)) {}

  // get the load value for the key
  val_t get(const key_t& key) { return cache_.get_value(key); }
  // insert a new key, make it to first position
  void set(const key_t& key, const val_t& val) {  cache_.insert(key, val); }

  // testing purposes
  // return the newest key
  key_t get_newest_key() const { return cache_.get_newest_key(); }
  // return oldest key, no position updates
  key_t get_oldest_key() const { return cache_.get_oldest_key(); } 

private:
  CacheTable cache_;
};
