#pragma once

#include "HashMap.h"
#include "TreeMap.h"
#include "MapHuff.h"
#include <memory>

namespace Structure {

enum class MapType {
    HASH_MAP,
    TREE_MAP
};

template <typename K, typename V>
class MapFactory {
public:
    static MapHuff<K, V>* createMap(MapType type) {
        switch (type) {
            case MapType::HASH_MAP:
                return new HashMap<K, V>();
            case MapType::TREE_MAP:
                return new TreeMap<K, V>();
            default:
                return nullptr;
        }
    }
};

}
