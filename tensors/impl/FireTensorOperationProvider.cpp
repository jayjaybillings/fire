#if 0
#include "FireTensorOperationProvider.hpp"

namespace fire {
TensorReference& FireTensorOperationProvider::executeBinaryOperation(
		TensorOperation op, TensorReference& first,
		TensorReference& second) {
}

TensorReference& FireTensorOperationProvider::executeUnaryOperation(
			TensorOperation op, TensorReference& tensor) {

}

}
REGISTER_TENSORPROVIDER(fire::FireTensorOperationProvider, "fire");
#endif
