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
#define BOOST_TEST_MODULE Tensors

#include <boost/test/included/unit_test.hpp>
#include "Tensor.hpp"
#include "EigenTensorOperationProvider.hpp"
#include <unsupported/Eigen/CXX11/Tensor>

using namespace boost;

BOOST_AUTO_TEST_CASE(checkConstruction) {

	fire::Tensor<fire::EigenTensorOperationProvider<5>> a(1, 2, 3, 4, 5);

	BOOST_VERIFY (a.dimension(0) == 1);
	BOOST_VERIFY (a.dimension(1) == 2);
	BOOST_VERIFY (a.dimension(2) == 3);
	BOOST_VERIFY (a.dimension(3) == 4);
	BOOST_VERIFY (a.dimension(4) == 5);

	int counter = 0;
	for (int i = 0; i < 1; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 4; l++) {
					for (int m = 0; m < 5; m++) {
						counter++;
						BOOST_VERIFY(a(i,j,k,l,m) == 0.0);
					}
				}
			}
		}
	}
	BOOST_VERIFY(counter == 120);

	fire::Tensor<fire::EigenTensorOperationProvider<3>> epsilon(3,3,3);
	epsilon(0,1,2) = 1;
	BOOST_VERIFY(epsilon(0,1,2) == 1);
	epsilon(1,2,0) = 1;
	BOOST_VERIFY(epsilon(1,2,0) == 1);
	epsilon(2,0,1) = 1;
	BOOST_VERIFY(epsilon(2,0,1) == 1);
	epsilon(1,0,2) = -1;
	BOOST_VERIFY(epsilon(1,0,2) == -1);
	epsilon(2,1,0) = -1;
	BOOST_VERIFY(epsilon(2,1,0) == -1);
	epsilon(0,2,1) = -1;
	BOOST_VERIFY(epsilon(0,2,1) == -1);

	fire::Tensor<fire::EigenTensorOperationProvider<4>> grassmannIdentity(3,3,3,3);
	for (int i = 0; i < 3; i++) {
	  for (int j = 0; j < 3; j++) {
	    for (int k = 0; k < 3; k++) {
	      for (int l = 0; l < 3; l++) {
	        for (int m = 0; m < 3; m++) {
	          grassmannIdentity(i,j,l,m) += epsilon(i,j,k) * epsilon(k,l,m);
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
	        BOOST_VERIFY(grassmannIdentity(i,j,l,m) == (int(i == l) * int(j == m) - int(i == m) * int(j == l)));
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
	fire::Tensor<fire::EigenTensorOperationProvider<2>> a(2,2);
	fire::Tensor<fire::EigenTensorOperationProvider<2>> b(2,2);

	a(0,0) = 1;
	b(0,0) = 1;

	auto result = a + b;

	BOOST_VERIFY(result.rank() == 2);
	BOOST_VERIFY(result.dimension(0) == 2);
	BOOST_VERIFY(result.dimension(1) == 2);
	BOOST_VERIFY(result(0,0) == 2);
	BOOST_VERIFY(result(0,1) == 0);
	BOOST_VERIFY(result(1,0) == 0);
	BOOST_VERIFY(result(1,1) == 0);

	fire::Tensor<fire::EigenTensorOperationProvider<2>> expected(2,2);

}
