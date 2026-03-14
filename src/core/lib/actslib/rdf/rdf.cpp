#include "rdf.hpp"

namespace actslib::rdf {
	namespace {
		const char* rdfnames[]{
			"subject",
			"predicate",
			"object",
			"graph",
		};
	}

	const char* GetRDFComponentTypeName(RDFComponentType type) {
		if (type >= RDF_QUAD_COUNT || type < 0) {
			return "unknown";
		}
		return rdfnames[type];
	}
}