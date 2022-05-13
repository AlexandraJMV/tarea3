#ifndef TREEMAP_h
#define TREEMAP_h

typedef struct TreeMap TreeMap;

typedef struct TreePair {
     void * key;
     void * value;
} TreePair;

TreeMap * createTreeMap(int (*lower_than_int) (void* key1, void* key2));

void insertTreeMap(TreeMap * tree, void* key, void * value);

void eraseTreeMap(TreeMap * tree, void* key);

TreePair * searchTreeMap(TreeMap * tree, void* key);

TreePair * upperBound(TreeMap * tree, void* key);

TreePair * firstTreeMap(TreeMap * tree);

TreePair * nextTreeMap(TreeMap * tree);

#endif /* TREEMAP_h */
