#ifndef SHARED_NAMES
#define SHARED_NAMES

typedef struct SharedNames
{
    char* shm_;
    char* semMutex_;
} SharedNames;

void clearNames(SharedNames* names);
char * add_suffix(const char * name, const char * suffix);
void createNames(const char* suffix, SharedNames names);
#endif 