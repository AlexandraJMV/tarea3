#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct Pair{
    void * key;
    void * value;
}Pair;

typedef struct HashMap HashMap;

void insertMap(HashMap * map, char * key, void * value);

HashMap * createMap(long capacity);

void eraseMap(HashMap * map,  char * key) ;

Pair * searchMap(HashMap * map,  char * key);

Pair * firstMap(HashMap * map);

Pair * nextMap(HashMap * map);


#endif /* HASHMAP_H */