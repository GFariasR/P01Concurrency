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
	// modify the values (instead of delete and new again)
	oldNode->set_values(key, val);
	// make this node the newest now
	pos_list_.update_to_newest(oldNode);     
	// safe to remove the key, its node is saved in  oldNode
	map_.erase(iteErase);            
	// inserted back in the map with new key and value
        map_.emplace(key, oldNode);          
      } else {
	throw "this is weird:"
	      " no posible to erase a key present on the posList?";
      }
    } else {   //  there is room in cache, so add it
      // creates a new node
      PositionNode* posNode = new PositionNode(key, val);  
      pos_list_.set_newest(posNode);             // this is the newest now
      // into the map now
      map_.emplace(key, posNode);
    }
  } else {
    // isInMap found in map, we need to update its position to newest...
    // ...and the value
    PositionNode* theNode = isInMap->second;
    theNode->set_value(val);
    pos_list_.update_to_newest(theNode);
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
    pos_list_.update_to_newest(isInMap->second);
  }
  return isInMap->second->get_value();
}

// has to destroy all the position Nodes in the map
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
