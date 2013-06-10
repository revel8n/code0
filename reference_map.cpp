#include "reference_map.h"

// class CReferenceMap

std::pair<CReferenceMap::REFERENCE_FORWARD_ITERATOR,CReferenceMap::REFERENCE_FORWARD_ITERATOR> CReferenceMap::QueryFromAddress(CYBER_ADDRESS Address)
{
	return std::make_pair(mSetForward.lower_bound
}
