#include "searchspacepool.h"

namespace gaia2 {

// static variable for normal SearchSpaces
template<>
QMutex* SearchSpacePool::_sspoolMutex = 0;

template<>
QList<SearchSpace*> SearchSpacePool::_sspool = QList<SearchSpace*>();


// static variable for frozen SearchSpaces
template<>
QMutex* FrozenSearchSpacePool::_sspoolMutex = 0;

template<>
QList<FrozenSearchSpace*> FrozenSearchSpacePool::_sspool = QList<FrozenSearchSpace*>();



} // namespace gaia2
