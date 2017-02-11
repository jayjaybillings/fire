/**----------------------------------------------------------------------------
 Copyright (c) 2016-, UT-Battelle LLC
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 * Neither the name of fern nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 Author(s): Alex McCaskey (mccaskeyaj <at> ornl <dot> gov)
 -----------------------------------------------------------------------------*/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE FireTensors

#include <boost/test/included/unit_test.hpp>
#include "Tensor.hpp"

using namespace boost;

template<const int Rank>
using FireTensor = fire::Tensor<Rank, fire::EigenProvider, double>;

BOOST_AUTO_TEST_CASE(checkConstruction) {

	FireTensor<5> a(1, 2, 3, 4, 5);

	BOOST_VERIFY(a.dimension(0) == 1);
	BOOST_VERIFY(a.dimension(1) == 2);
	BOOST_VERIFY(a.dimension(2) == 3);
	BOOST_VERIFY(a.dimension(3) == 4);
	BOOST_VERIFY(a.dimension(4) == 5);

	int counter = 0;
	for (int i = 0; i < 1; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 4; l++) {
					for (int m = 0; m < 5; m++) {
						counter++;
						BOOST_VERIFY(a(i, j, k, l, m) == 0.0);
					}
				}
			}
		}
	}
	BOOST_VERIFY(counter == 120);

	FireTensor<3> epsilon(3, 3, 3);
	epsilon(0, 1, 2) = 1;
	BOOST_VERIFY(epsilon(0, 1, 2) == 1);
	epsilon(1, 2, 0) = 1;
	BOOST_VERIFY(epsilon(1, 2, 0) == 1);
	epsilon(2, 0, 1) = 1;
	BOOST_VERIFY(epsilon(2, 0, 1) == 1);
	epsilon(1, 0, 2) = -1;
	BOOST_VERIFY(epsilon(1, 0, 2) == -1);
	epsilon(2, 1, 0) = -1;
	BOOST_VERIFY(epsilon(2, 1, 0) == -1);
	epsilon(0, 2, 1) = -1;
	BOOST_VERIFY(epsilon(0, 2, 1) == -1);

	FireTensor<4> grassmannIdentity(3, 3, 3, 3);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 3; l++) {
					for (int m = 0; m < 3; m++) {
						grassmannIdentity(i, j, l, m) += epsilon(i, j, k)
								* epsilon(k, l, m);
					}
				}
			}
		}
	}

	// verify
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int l = 0; l < 3; l++) {
				for (int m = 0; m < 3; m++) {
					BOOST_VERIFY(
							grassmannIdentity(i, j, l, m)
									== (int(i == l) * int(j == m)
											- int(i == m) * int(j == l)));
				}
			}
		}
	}

	// dimensionalities
	BOOST_VERIFY(epsilon.dimension(0) == 3);
	BOOST_VERIFY(epsilon.dimension(1) == 3);
	BOOST_VERIFY(epsilon.dimension(2) == 3);
}

BOOST_AUTO_TEST_CASE(checkAddition) {
	FireTensor<2> a(2, 2);
	FireTensor<2> b(2, 2);

	a(0, 0) = 1;
	b(0, 0) = 1;

	FireTensor<2> result = a + b;

	BOOST_VERIFY(result.getRank() == 2);
	BOOST_VERIFY(result.dimension(0) == 2);
	BOOST_VERIFY(result.dimension(1) == 2);
	BOOST_VERIFY(result(0, 0) == 2);
	BOOST_VERIFY(result(0, 1) == 0);
	BOOST_VERIFY(result(1, 0) == 0);
	BOOST_VERIFY(result(1, 1) == 0);

	FireTensor<2> expected(2, 2);
}

BOOST_AUTO_TEST_CASE(checkEquality) {
	FireTensor<2> a(2, 2);
	FireTensor<2> b(2, 2);

	a(0, 0) = 1;
	b(0, 0) = 1;

	BOOST_VERIFY(a == b);

	a(0, 0) = 2;

	BOOST_VERIFY(!(a == b));
	BOOST_VERIFY(a != b);
}

BOOST_AUTO_TEST_CASE(checkContraction) {

	using namespace fire;
	using IndexPair = std::pair<int,int>;

	FireTensor<2> mat1(2, 3);
	FireTensor<2> mat2(2, 3);

	mat1.setRandom();
	mat2.setRandom();

	FireTensor<2> mat4(3, 3);

	std::array<IndexPair, 1> contractionIndices;
	contractionIndices[0] = std::make_pair(0, 0);
	mat4 = mat1.contract(mat2, contractionIndices);

	BOOST_VERIFY(
			mat4(0, 0) == (mat1(0, 0) * mat2(0, 0) + mat1(1, 0) * mat2(1, 0)));
	BOOST_VERIFY(
			mat4(0, 1) == (mat1(0, 0) * mat2(0, 1) + mat1(1, 0) * mat2(1, 1)));
	BOOST_VERIFY(
			mat4(0, 2) == (mat1(0, 0) * mat2(0, 2) + mat1(1, 0) * mat2(1, 2)));
	BOOST_VERIFY(
			mat4(1, 0) == (mat1(0, 1) * mat2(0, 0) + mat1(1, 1) * mat2(1, 0)));
	BOOST_VERIFY(
			mat4(1, 1) == (mat1(0, 1) * mat2(0, 1) + mat1(1, 1) * mat2(1, 1)));
	BOOST_VERIFY(
			mat4(1, 2) == (mat1(0, 1) * mat2(0, 2) + mat1(1, 1) * mat2(1, 2)));
	BOOST_VERIFY(
			mat4(2, 0) == (mat1(0, 2) * mat2(0, 0) + mat1(1, 2) * mat2(1, 0)));
	BOOST_VERIFY(
			mat4(2, 1) == (mat1(0, 2) * mat2(0, 1) + mat1(1, 2) * mat2(1, 1)));
	BOOST_VERIFY(
			mat4(2, 2) == (mat1(0, 2) * mat2(0, 2) + mat1(1, 2) * mat2(1, 2)));

}

BOOST_AUTO_TEST_CASE(checkFireTensorProduct) {
	using namespace fire;
	using IndexPair = std::pair<int,int>;

	FireTensor<2> mat1(2, 3);
	FireTensor<2> mat2(4, 1);
	mat1.setRandom();
	mat2.setRandom();

	// Note user must know tensor product produces rank n*m
	FireTensor<4> result = mat1 * mat2;

	BOOST_VERIFY(result.dimension(0) == 2);
	BOOST_VERIFY(result.dimension(1) == 3);
	BOOST_VERIFY(result.dimension(2) == 4);
	BOOST_VERIFY(result.dimension(3) == 1);
	for (int i = 0; i < result.dimension(0); ++i) {
		for (int j = 0; j < result.dimension(1); ++j) {
			for (int k = 0; k < result.dimension(2); ++k) {
				for (int l = 0; l < result.dimension(3); ++l) {
					BOOST_VERIFY(result(i, j, k, l) == mat1(i, j) * mat2(k, l));
				}
			}
		}
	}
}

BOOST_AUTO_TEST_CASE(checkSetValuesInitializerList) {
	using namespace fire;

	FireTensor<2> t(2, 3);

	t.setValues( { { 0, 1, 2 }, { 3, 4, 5 } });

	BOOST_VERIFY(t(0, 0) == 0);
	BOOST_VERIFY(t(0, 1) == 1);
	BOOST_VERIFY(t(0, 2) == 2);
	BOOST_VERIFY(t(1, 0) == 3);
	BOOST_VERIFY(t(1, 1) == 4);
	BOOST_VERIFY(t(1, 2) == 5);

//	std::cout
//			<< "Checking setValues with initializer_list.\nFireTensor<2>(2,3) = \n";
//	t.print(std::cout);
//	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(checkScalarMultiply) {
	using namespace fire;

	FireTensor<2> s(2, 3);

	s.setValues( { { 0, 1, 2 }, { 3, 4, 5 } });

	FireTensor<2> t = s * 2.0;

	BOOST_VERIFY(t(0, 0) == 0);
	BOOST_VERIFY(t(0, 1) == 2);
	BOOST_VERIFY(t(0, 2) == 4);
	BOOST_VERIFY(t(1, 0) == 6);
	BOOST_VERIFY(t(1, 1) == 8);
	BOOST_VERIFY(t(1, 2) == 10);

//	std::cout
//			<< "Checking setValues with initializer_list.\nFireTensor<2>(2,3) = \n";
//	t.print(std::cout);

}

BOOST_AUTO_TEST_CASE(checkFireTensorReshapeAndShuffle) {
	using namespace fire;

	FireTensor<2> tensor(7, 11);

	std::array<int, 3> newShape { { 7, 11, 1 } };

	FireTensor<3> reShapedFireTensor = tensor.reshape(newShape);

	BOOST_VERIFY(reShapedFireTensor.getRank() == 3);
	BOOST_VERIFY(reShapedFireTensor.dimension(0) == 7);
	BOOST_VERIFY(reShapedFireTensor.dimension(1) == 11);
	BOOST_VERIFY(reShapedFireTensor.dimension(2) == 1);

	FireTensor<3> input(20, 30, 50);
	input.setRandom();

	std::array<int, 3> permutation { { 1, 2, 0 } };
	FireTensor<3> output = input.shuffle(permutation);
	BOOST_VERIFY(output.dimension(0) == 30);
	BOOST_VERIFY(output.dimension(1) == 50);
	BOOST_VERIFY(output.dimension(2) == 20);

	BOOST_VERIFY(output(3, 7, 11) == input(11, 3, 7));
}

BOOST_AUTO_TEST_CASE(checkSVD) {
	using namespace fire;
	using IndexPair = std::pair<int,int>;

	// Create a Rank 4 tensor - the GHZ state...
	FireTensor<4> tensor(2, 2, 2, 2);
	tensor(0, 0, 0, 0) = 1.0 / sqrt(2.0);
	tensor(1, 1, 1, 1) = 1.0 / sqrt(2.0);

	// Setup the SVD Cut
	std::array<int, 2> leftCut { 0, 1 }, rightCut { 2, 3 };

	// Perform the SVD, this gives us a U, S, and V tensor
	// with the default truncation of 0.0
	auto result = tensor.svd(leftCut, rightCut);

	// Get them individually...
	auto U = std::get<0>(result);
	auto S = std::get<1>(result);
	auto V = std::get<2>(result);

	// Now let's verify the result. Contracting
	// U, S, V with the original tensor should
	// result in a value of 1.0

	// Contract U and S
	std::array<IndexPair, 1> dims;
	dims[0] = std::make_pair(2, 0);
	auto us = U.contract(S, dims);
	BOOST_VERIFY(us.getRank() == 3);

	// Contract new U and V
	dims[0] = std::make_pair(2, 2);
	auto uv = us.contract(V, dims);
	BOOST_VERIFY(uv.getRank() == 4);

	// Now contract the UV rank 4 tensor
	// with the original GHZ rank 4 tensor
	std::array<IndexPair, 4> totalDims { std::make_pair(0, 0), std::make_pair(1,
			1), std::make_pair(2, 2), std::make_pair(3, 3) };
	auto scalarFireTensor = uv.contract(tensor, totalDims);
	BOOST_VERIFY(scalarFireTensor.getRank() == 0);
	BOOST_VERIFY(fabs(scalarFireTensor() - 1) < 1e-6);

	// Test a more complicated tensor with random values...

	// Create a Rank 4 random tensor - but it has to be
	// orthonormal to perform our contraction == 1.0 test.
	// Start off by creating a 4x4 matrix that has
	// orthogonal columns (taken from blogs.sas.com/content/iml/2012/03/28/generating-a-random-orthogonal-matrix.html)
	Eigen::MatrixXd m(4,4);
	m << .1427043 , -.386502 , -.123226 , 0.9028106 ,.1922906 , .345096 , -.918623 , -.00804 ,
			.3633545 , .7887484 , .3694716 , .3306664 ,-0.900352 , .3307579 , -.066617 , .2748238;

	// Now normalize that matrix, and create a rank 4 tensor from the
	// data...
	std::vector<int> r4shape = {2, 2, 2, 2};
	m = (1.0 / m.norm()) * m;
	TensorShape shape(r4shape);
	auto ref = fire::make_tensor_reference(m.data(), shape);
	FireTensor<4> randomFireTensor(ref);

	// Perform the SVD, this gives us a U, S, and V tensor
	// with the default truncation of 0.0
	auto randomResult = randomFireTensor.svd(leftCut, rightCut);

	// Get them individually...
	auto randomU = std::get<0>(randomResult);
	auto randomS = std::get<1>(randomResult);
	auto randomV = std::get<2>(randomResult);

	// Contract randomU and randomS
	dims[0] = std::make_pair(2, 0);
	auto randomus = randomU.contract(randomS, dims);
	BOOST_VERIFY(randomus.getRank() == 3);

	// Contract new randomus and randomV
	dims[0] = std::make_pair(2, 2);
	auto randomuv = randomus.contract(randomV, dims);
	BOOST_VERIFY(randomuv.getRank() == 4);

	// Now contract the randomuv rank 4 tensor
	// with the original random rank 4 tensor
	auto randomScalarFireTensor = randomuv.contract(randomFireTensor, totalDims);
	BOOST_VERIFY(randomScalarFireTensor.getRank() == 0);
	BOOST_VERIFY(fabs(randomScalarFireTensor() - 1) < 1e-6);

}

BOOST_AUTO_TEST_CASE(checkTransposeRank2) {
	FireTensor<2> a(2,2);
	a.setValues({{0,1},{2,0}});
	auto b = a.transpose();
	BOOST_VERIFY(b(0,1) == 2);
	BOOST_VERIFY(b(1,0) == 1);
}

BOOST_AUTO_TEST_CASE(checkKronProd) {
	FireTensor<2> x(2,2), I(2,2);
	x.setValues({{0,1},{1,0}});
	I.setValues({{1,0},{0,1}});

	FireTensor<2> result = x.kronProd(I);

	BOOST_VERIFY(result.dimension(0) == 4);
	BOOST_VERIFY(result.dimension(1) == 4);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if ((i == 0 && j == 2) ||
					(i == 1 && j == 3) ||
					(i == 2 && j == 0) ||
					(i == 3 && j == 1)) {
				BOOST_VERIFY(result(i,j) == 1);
			} else {
				BOOST_VERIFY(result(i,j) == 0);
			}
		}
	}
}

BOOST_AUTO_TEST_CASE(checkRank1OuterProduct) {
	FireTensor<1> vec(4);
	vec.setValues({1.0/std::sqrt(2.0), 0, 0, 1.0/std::sqrt(2.0)});
	auto rho = vec * vec;
	BOOST_VERIFY(rho.getRank() == 2);
	BOOST_VERIFY(rho.dimension(0) == 4);
	BOOST_VERIFY(rho.dimension(1) == 4);

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if ((i == 0 && j == 0) ||
					(i == 0 && j == 3) ||
					(i == 3 && j == 0) ||
					(i == 3 && j == 3)) {
				BOOST_VERIFY(fabs(rho(i,j) - 0.5) < 1e-3);
			} else {
				BOOST_VERIFY(rho(i,j) == 0);
			}
		}
	}
}

template<const int Rank>
using ComplexFireTensor = fire::Tensor<Rank, fire::EigenProvider, std::complex<double>>;

BOOST_AUTO_TEST_CASE(checkComplexConstruction) {

	ComplexFireTensor<5> a(1, 2, 3, 4, 5);

	BOOST_VERIFY(a.dimension(0) == 1);
	BOOST_VERIFY(a.dimension(1) == 2);
	BOOST_VERIFY(a.dimension(2) == 3);
	BOOST_VERIFY(a.dimension(3) == 4);
	BOOST_VERIFY(a.dimension(4) == 5);

	int counter = 0;
	for (int i = 0; i < 1; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 4; l++) {
					for (int m = 0; m < 5; m++) {
						counter++;
						BOOST_VERIFY(a(i, j, k, l, m) == 0.0);
					}
				}
			}
		}
	}
	BOOST_VERIFY(counter == 120);

	ComplexFireTensor<3> epsilon(3, 3, 3);
	epsilon(0, 1, 2) = 1;
	BOOST_VERIFY(std::real(epsilon(0, 1, 2)) == 1);
	epsilon(1, 2, 0) = 1;
	BOOST_VERIFY(std::real(epsilon(1, 2, 0)) == 1);
	epsilon(2, 0, 1) = 1;
	BOOST_VERIFY(std::real(epsilon(2, 0, 1)) == 1);
	epsilon(1, 0, 2) = -1;
	BOOST_VERIFY(std::real(epsilon(1, 0, 2)) == -1);
	epsilon(2, 1, 0) = -1;
	BOOST_VERIFY(std::real(epsilon(2, 1, 0)) == -1);
	epsilon(0, 2, 1) = -1;
	BOOST_VERIFY(std::real(epsilon(0, 2, 1)) == -1);

	ComplexFireTensor<4> grassmannIdentity(3, 3, 3, 3);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 3; l++) {
					for (int m = 0; m < 3; m++) {
						grassmannIdentity(i, j, l, m) += epsilon(i, j, k)
								* epsilon(k, l, m);
					}
				}
			}
		}
	}

	// verify
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			for (int l = 0; l < 3; l++) {
				for (int m = 0; m < 3; m++) {
					BOOST_VERIFY(
							std::real(grassmannIdentity(i, j, l, m))
									== (int(i == l) * int(j == m)
											- int(i == m) * int(j == l)));
				}
			}
		}
	}

	// dimensionalities
	BOOST_VERIFY(epsilon.dimension(0) == 3);
	BOOST_VERIFY(epsilon.dimension(1) == 3);
	BOOST_VERIFY(epsilon.dimension(2) == 3);
}


BOOST_AUTO_TEST_CASE(checkComplexAddition) {
	ComplexFireTensor<2> a(2, 2);
	ComplexFireTensor<2> b(2, 2);

	a(0, 0) = 1;
	b(0, 0) = 1;

	ComplexFireTensor<2> result = a + b;

	BOOST_VERIFY(result.getRank() == 2);
	BOOST_VERIFY(result.dimension(0) == 2);
	BOOST_VERIFY(result.dimension(1) == 2);
	BOOST_VERIFY(std::real(result(0, 0)) == 2);
	BOOST_VERIFY(std::real(result(0, 1)) == 0);
	BOOST_VERIFY(std::real(result(1, 0)) == 0);
	BOOST_VERIFY(std::real(result(1, 1)) == 0);

}

BOOST_AUTO_TEST_CASE(checkComplexEquality) {
	ComplexFireTensor<2> a(2, 2);
	ComplexFireTensor<2> b(2, 2);

	a(0, 0) = 1;
	b(0, 0) = 1;

	BOOST_VERIFY(a == b);

	a(0, 0) = 2;

	BOOST_VERIFY(!(a == b));
	BOOST_VERIFY(a != b);
}

BOOST_AUTO_TEST_CASE(checkComplexContraction) {

//	using namespace fire;
//	using IndexPair = std::pair<int,int>;
//
//	ComplexFireTensor<2> t_left(30, 30)
//	ComplexFireTensor<2> t_right(30, 30);
//	ComplexFireTensor<2> t_result(30, 30);
//
//	t_left.setRandom();
//	t_right.setRandom();
//
//	using namespace Eigen;
//
//	typedef Map<Matrix<std::complex<double>, Dynamic, Dynamic>> MapXcf;
//	MapXcf m_left(t_left.createReference().first.data(), 1500, 248);
//	MapXcf m_right(t_right.createReference().first.data(), 248, 1400);
//	Matrix<std::complex<double>, Dynamic, Dynamic> m_result(1500, 1400);
//
//	using DimPair = Eigen::Tensor<float, 1>::DimensionPair;
//	Eigen::array<DimPair, 2> dims;
//	dims[0] = DimPair(2,0);
//	dims[1] = DimPair(3,1);
//
//	// This contraction should be equivalent to a regular matrix multiplication
////	std::array<IndexPair, 2> dims;
////	dims[0] = std::make_pair(2, 0);
////	dims[1] = std::make_pair(3, 1);
//	t_result = t_left.contract(t_right, dims);
//	m_result = m_left * m_right;
//	auto ref = t_result.createReference();
//	for (int i = 0; i < t_result.size(); i++) {
//		auto diff = ref.first.data()[i] - m_result.data()[i];
//		BOOST_VERIFY(fabs(std::real(diff)) < 1e-6);
//	}

}


BOOST_AUTO_TEST_CASE(checkComplexFireTensorProduct) {
	using namespace fire;
	using IndexPair = std::pair<int,int>;

	ComplexFireTensor<2> mat1(2, 3);
	ComplexFireTensor<2> mat2(4, 1);
	mat1.setRandom();
	mat2.setRandom();

	// Note user must know tensor product produces rank n*m
	ComplexFireTensor<4> result = mat1 * mat2;

	BOOST_VERIFY(result.dimension(0) == 2);
	BOOST_VERIFY(result.dimension(1) == 3);
	BOOST_VERIFY(result.dimension(2) == 4);
	BOOST_VERIFY(result.dimension(3) == 1);
	for (int i = 0; i < result.dimension(0); ++i) {
		for (int j = 0; j < result.dimension(1); ++j) {
			for (int k = 0; k < result.dimension(2); ++k) {
				for (int l = 0; l < result.dimension(3); ++l) {
					BOOST_VERIFY(result(i, j, k, l) == mat1(i, j) * mat2(k, l));
				}
			}
		}
	}
}

BOOST_AUTO_TEST_CASE(checkComplexSetValuesInitializerList) {
	using namespace fire;

	ComplexFireTensor<2> t(2, 3);

	t.setValues( { { 0, 1, 2 }, { 3, 4, 5 } });

	BOOST_VERIFY(std::real(t(0, 0)) == 0);
	BOOST_VERIFY(std::real(t(0, 1)) == 1);
	BOOST_VERIFY(std::real(t(0, 2)) == 2);
	BOOST_VERIFY(std::real(t(1, 0)) == 3);
	BOOST_VERIFY(std::real(t(1, 1)) == 4);
	BOOST_VERIFY(std::real(t(1, 2)) == 5);

//	std::cout
//			<< "Checking setValues with initializer_list.\nFireTensor<2>(2,3) = \n";
//	t.print(std::cout);
//	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(checkComplexScalarMultiply) {
	using namespace fire;

	ComplexFireTensor<2> s(2, 3);

	s.setValues( { { 0, 1, 2 }, { 3, 4, 5 } });

	ComplexFireTensor<2> t = s * 2.0;

	BOOST_VERIFY(std::real(t(0, 0)) == 0);
	BOOST_VERIFY(std::real(t(0, 1)) == 2);
	BOOST_VERIFY(std::real(t(0, 2)) == 4);
	BOOST_VERIFY(std::real(t(1, 0)) == 6);
	BOOST_VERIFY(std::real(t(1, 1)) == 8);
	BOOST_VERIFY(std::real(t(1, 2)) == 10);

//	std::cout
//			<< "Checking setValues with initializer_list.\nFireTensor<2>(2,3) = \n";
//	t.print(std::cout);

}

BOOST_AUTO_TEST_CASE(checkComplexFireTensorReshapeAndShuffle) {
	using namespace fire;

	ComplexFireTensor<2> tensor(7, 11);

	std::array<int, 3> newShape { { 7, 11, 1 } };

	ComplexFireTensor<3> reShapedFireTensor = tensor.reshape(newShape);

	BOOST_VERIFY(reShapedFireTensor.getRank() == 3);
	BOOST_VERIFY(reShapedFireTensor.dimension(0) == 7);
	BOOST_VERIFY(reShapedFireTensor.dimension(1) == 11);
	BOOST_VERIFY(reShapedFireTensor.dimension(2) == 1);

	ComplexFireTensor<3> input(20, 30, 50);
	input.setRandom();

	std::array<int, 3> permutation { { 1, 2, 0 } };
	ComplexFireTensor<3> output = input.shuffle(permutation);
	BOOST_VERIFY(output.dimension(0) == 30);
	BOOST_VERIFY(output.dimension(1) == 50);
	BOOST_VERIFY(output.dimension(2) == 20);

	BOOST_VERIFY(output(3, 7, 11) == input(11, 3, 7));
}
