#ifndef BMAIN_H
#define BMAIN_H

#include <mutex>

extern std::mutex actorsLock;
extern std::mutex playersLock;
void bmain();

#endif /* BMAIN_H */
