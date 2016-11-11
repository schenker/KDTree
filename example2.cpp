#include <iostream>
#include "KDTree.hpp"

#include <string>

// this time we are in a three-dimensional space
const int dim = 3;

// our tree will contain elements of type P.
// the three dimensions of P have different types.
struct P {
	int d0; // first dimensions type is int
	double d1; // second dimensions type is double
	std::string d2; // third dimensions type is std::string

	P(int d0, double d1, std::string d2) : d0(d0), d1(d1), d2(d2) {}
};

// we will pass this functor as a template parameter to the kdtree.
// this is used by the kdtree to create new instances of P.
struct P_new {
	P *operator()() const {
		return new P(0, 0, "");
	}
};

// we will pass this functor as a template parameter to the kdtree.
// this is used by the kdtree to delete the instances of P that have been created with P_new.
struct P_delete {
	void operator()(P* p) const {
		delete p;
	}
};

// we will pass this functor as a template parameter to the kdtree.
// this is used for comparisons of instances of P on different dimensions .
struct P_less {
	bool operator() (P const * const a, P const * const b, unsigned int dim) const {
		// depending on dim, we consider different dimensions of P
		if (dim == 0) {
			return std::less<int>()(a->d0, b->d0);
		}
		else if (dim == 1) {
			return std::less<double>()(a->d1, b->d1);
		}
		else if (dim == 2) {
			return std::less<std::string>()(a->d2, b->d2);
		}
	}
};

// we will pass this functor as a template parameter to the kdtree.
// this is used to copy values from one instance of P to another instance of P
struct P_set {
	void operator()(P * destination, P const * const source, const unsigned int dim) const {
		// depending on dim, we consider different dimensions of P
		if (dim == 0) {
			destination->d0 = source->d0;
		}
		else if (dim == 1) {
			destination->d1 = source->d1;
		}
		else if (dim == 2) {
			destination->d2 = source->d2;
		}
	}
};

int main() {
	// prepare elements, three dimensional P objects
	P p1(0, 2, "a");
	P p2(1, 2, "b");
	P p3(1, 2, "c");
	P p4(1, 7, "d");
	P p5(1, 2, "e");

	// prepare the entries
	std::vector<P*> values;
	values.push_back(&p1);
	values.push_back(&p2);
	values.push_back(&p3);
	values.push_back(&p4);
	values.push_back(&p5);

	// the tree uses default implementations of new, delete, set and less for arrays.
	// since we don't use arrays, but the custom type P, we need to pass the functors we defined above to enable the tree to work with P
	kdtree::KDTree<P, dim, P_new, P_delete, P_set, P_less> tree(values);

	// prepare search query.
	// we search for P objects that match these criterias:
	//	d0 >= 0, d0 <= 2
	//	d1 >= 0, d1 <= 6
	//	d2 >= "b", d2 <= "d"
	P searchRectMin(0, 0, "b");
	P searchRectMax(2, 6, "d");

	// perform the search. result will contain the elements that match the query.
	std::vector<P*> result = tree.queryRectangle(&searchRectMin, &searchRectMax);

	// print results
	std::cout << "found " << result.size() << " entries:" << std::endl;
	for (std::vector<P*>::const_iterator it = result.begin(); it != result.end(); ++it) {
		std::cout << (*it)->d0 << " " << (*it)->d1 << " " << (*it)->d2 << std::endl;
	}

	return 1;
}
