#include "LRUCache.hpp"

/**********************************************************************
 *** CACHE TABLE ******************************************************
 **********************************************************************/
void CacheTable::insert(const key_t& key, const val_t& val)
{
  if (key < 0) {
    return;    // what are you doing man!
  }
  
  // first we need to know if already exists
  auto isInMap = map_.find(key);
  if (isInMap == map_.end()) { 
      // ...  does not exist.. we need to manage capacity
    if (map_.size() == capacity_) {
      // we need to remove one pair first, the oldest
      // ...do not loose the posNode associated to the erased oldkey
      auto oldNode = pos_list_.get_oldest(); // get the oldest node
      // the key to be remove from the map
      key_t oldKey = oldNode->get_key();
      // get an iterator find the key on the map first
      auto iteErase = map_.find(oldKey);    
      if (iteErase != map_.end()) {
	LoadValue* lvptr = iteErase->second;
	// safe to remove the key, its node is saved in  oldNode
	map_.erase(iteErase);            
	oldNode->set_key(key);     // update the key on the old node
	lvptr->set_value(val);     // update the value
	lvptr->set_pos_node(oldNode);   // and the pos node
	// inserted back in the map with new key and value
        map_.emplace(key, lvptr);          
	// we got the new key in the cache...
	// ...but the position is the oldKey's one now, make it newest
	pos_list_.update_to_newest(oldNode);     
      } else {
	throw "this is weird:"
	      " no posible to erase a key present on the posList?";
      }
    } else {   //  there is room in cache, so add it
      PositionNode* posNode = new PositionNode(key);  // creates a new node
      pos_list_.set_newest(posNode);             // this is the newest now
      // creates a new load value 
      LoadValue* lv = new LoadValue(val, posNode);    
      map_.emplace(key, lv);
    }
  } else {
    // isInMap found in map, we need to update its position to newest...
    // ...and the value
    isInMap->second->set_value(val);
    pos_list_.update_to_newest(isInMap->second->get_pos_node());
  }
}

val_t CacheTable::get_value(const key_t& key)
{
  // check the map to get the value
  // if it does exist, update the position to newer
  // if does not exist, just returns -1
  auto isInMap = map_.find(key);
  if (isInMap == map_.end()) { // does not exist
    return -1;
  } else { // isInMap found in map, we need to update its position to newest
    pos_list_.update_to_newest(isInMap->second->get_pos_node());
  }
  return isInMap->second->get_value();
}

// has to destroy all the load values in the map
CacheTable::~CacheTable()
{
  if (!map_.empty()) {
    for (auto it : map_) {
      delete it.second;
      it.second = nullptr;
    }
    map_.clear();
  }
}

/**********************************************************************
 *** POSITION LIST  ***************************************************
 **********************************************************************/
PositionList::~PositionList()
{
  // has to destroy all the nodes
  while (oldest_ != nullptr) {
    auto tmp = oldest_->newer_;
    delete oldest_;
    oldest_ = tmp;
  }
  newest_ = nullptr;   
}

void PositionList::set_newest(PositionNode* posNode)
{
  if (newest_ == nullptr) {
    oldest_ = posNode;    // first node to be inserted
    newest_ = posNode;
    return;         // we are good nothing else to be done
  }
  newest_->newer_ = posNode;
  posNode->older_ = newest_;
  posNode->newer_ = nullptr;
  newest_ = posNode;
}

void PositionList::update_to_newest(PositionNode* posNode)
{
  // it could be the newest, nothing to do in that case
  if (posNode == newest_) {
    return;
  }
  // if could be the oldest, different treatment
  if (posNode == oldest_) {
    posNode->newer_->older_ = nullptr;    // make the previous, the older
    oldest_ = posNode->newer_;         
    set_newest(posNode);
  } else {
  // is a regular node 
    posNode->newer_->older_ = posNode->older_;
    posNode->older_->newer_ = posNode->newer_;
    set_newest(posNode);
  }
}
