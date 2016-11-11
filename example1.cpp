#include <iostream>
#include "KDTree.hpp"

#include <string>


int main() {

	const int dim = 5;

	// prepare elements, five dimensional int arrays. we could also use e.g. doubles if we wanted.
	int e1[dim] = {1, 1, 1, 1, 1};
	int e2[dim] = {1, 1, 1, 1, 1};
	int e3[dim] = {1, 2, 2, 2, 9};
	int e4[dim] = {1, 1, 1, 2, 1};
	int e5[dim] = {8, 8, 8, 8, 8};

	// fill the elements into a vector
	std::vector<int*> values;
	values.push_back(e1);
	values.push_back(e2);
	values.push_back(e3);
	values.push_back(e4);
	values.push_back(e5);

	// build the tree. first template parameter is the type of the elements, second parameter is the number of dimensions
	kdtree::KDTree<int, dim> tree(values);

	// prepare search query. we search for elements whose values are between 0 and 2 on all dimensions
	int searchRectMin[5] = {0,0,0,0,0};
	int searchRectMax[5] = {2,2,2,2,2};

	// perform the search. result will contain the elements that match the query.
	std::vector<int*> result = tree.queryRectangle(searchRectMin, searchRectMax);

	// print results
	std::cout << "found " << result.size() << " entries:" << std::endl;
	for(std::vector<int*>::const_iterator it = result.begin(); it != result.end(); ++it) {
		std::cout << (*it)[0] << " " << (*it)[1] << " " << (*it)[2] << " " << (*it)[3]<< " " << (*it)[4] << std::endl;
	}

	return 1;
}

