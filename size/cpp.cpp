

#include <hwloc.h>

class Cache{
  public:
    uint64_t size;
    uint16_t line_size;
};

std::unordered_map<uint16_t, Cache> caches;

static void check(hwloc_obj_t obj, int depth){
    for (size_t i = 0; i < obj->arity; i++) {
        check(obj->children[i], depth + 1);
    }
    auto insert = [=](const uint16_t t){
      if(!caches.count(t)){
        caches.emplace(t, Cache()).first->second.line_size = obj->attr->cache.linesize;
      }   
    };
    if (obj->type == HWLOC_OBJ_L3CACHE) insert(3);
    else
    if (obj->type == HWLOC_OBJ_L2CACHE) insert(2);
    else
    if (obj->type == HWLOC_OBJ_L1CACHE) insert(1);
}

int main(int argc, char* argv[]){

    hwloc_topology_t topo;
    hwloc_topology_init(&topo);
    hwloc_topology_load(topo);
    hwloc_obj_t obj;

    check(hwloc_get_root_obj(topo), 0);
    
    for (obj = hwloc_get_obj_by_type(topo, HWLOC_OBJ_PU, 0); obj; obj = obj->parent){
      if (obj->type == HWLOC_OBJ_L3CACHE) caches[3].size += obj->attr->cache.size;
      if (obj->type == HWLOC_OBJ_L2CACHE) caches[2].size += obj->attr->cache.size;
      if (obj->type == HWLOC_OBJ_L1CACHE) caches[1].size += obj->attr->cache.size;
    }

    hwloc_topology_destroy(topo);
    
//    for(const auto c : caches){
//      KLOG(INF) << c.first;
//      KLOG(INF) << c.second.size;
//      KLOG(INF) << c.second.line_size;    
//    }

    return 0;
}
