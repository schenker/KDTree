#ifndef __KDTREE_H__
#define __KDTREE_H__

#include <vector>
#include <algorithm>

namespace kdtree {

	/// @brief  default implementation of a functor that allocates a new tree element.
	/// if you want to use your own implementation instead, pass it to KDTree as the third template parameter.
	template <typename T, int dim> struct default_new {
		T *operator()() const {
			return new T[dim];
		}
	};

	/// @brief  default implementation of a functor that frees the memory of a tree element.
	/// if you want to use your own implementation instead, pass it to KDTree as the fourth template parameter.
	template <typename T> struct default_delete {
		void operator()(T* p) const {
			delete[] p;
		}
	};

	/// @brief default implementation of a functor that copies the value of one tree element to another tree element on one single dimension.
	/// if you want to use your own implementation instead, pass it to KDTree as the fifth template parameter.
	template <typename T> struct default_set {
		void operator()(T * const destination, T const * const source, const unsigned int dim) const {
			destination[dim] = source[dim];
		}
	};

	/// @brief default implementation of a functor that compares the values of two tree elements on one single dimension.
	/// if you want to use your own implementation instead, pass it to KDTree as the sixth template parameter.
	template <typename T> struct default_less {
		bool operator() (T const * const a, T const * const b, unsigned int dim) const {
			return std::less<T>()(a[dim], b[dim]);
		}
	};


	template <typename T, unsigned int dim,
		typename NEW = default_new<T, dim>, 
		typename DELETE = default_delete<T>,
		typename SET = default_set<T>,
		typename LESS = default_less<T> 
	> class KDTree {
	private:
		struct Node {
			Node * const left, *const right;
			T * value;

			Node(T *value, Node *leftNode = 0, Node *rightNode = 0) : value(value), left(leftNode), right(rightNode) {}
		} *root;

		T *boundingBoxMin = NEW()();
		T *boundingBoxMax = NEW()();

		void updateBoundingBox(const Node * const node) {
			for (int d = 0; d < dim; d++) {
				if (LESS()(node->value, boundingBoxMin, d)) {
					SET()(boundingBoxMin, node->value, d);
				}
				else if (LESS()(boundingBoxMax, node->value, d)) {
					SET()(boundingBoxMax, node->value, d);
				}
			}
		}

		Node *buildKDTree(std::vector<T*> &values, const unsigned int depth = 0) {
			Node *result;

			const int numberOfValues = values.size();

			if (numberOfValues == 1) {
				result = new Node(values.front());
			}
			else if (numberOfValues > 0) {

				std::partial_sort(values.begin(), values.begin() + (numberOfValues / 2) + 1, values.end(),
					[&depth](T *i, T *j) {
					return LESS()(i, j, depth%dim);
				});

				const int pivot = numberOfValues / 2;

				std::vector<T*> leftValues(values.begin(), values.begin() + pivot);
				std::vector<T*> rightValues(values.begin() + pivot, values.end());

				Node *leftNode = buildKDTree(leftValues, depth + 1);
				Node *rightNode = buildKDTree(rightValues, depth + 1);

				result = new Node(values.at(pivot), leftNode, rightNode);
			}
			else { // values.empty()
				result = 0;
			}

			updateBoundingBox(result);

			return result;
		}

		/// @brief adds all values contained in the subtree of node to the result vector.
		void reportSubtree(const Node * const node, std::vector<T*> * const result) const {
			if (node->left != 0) {
				reportSubtree(node->left, result);
				reportSubtree(node->right, result);
			}
			else {
				result->push_back(node->value);
			}
		}

		/// @brief checks wether the search region fully contains the other region.
		/// @returns true, if the other region is fully contained in the search region, false otherwise.
		bool regionContained(const T * const searchMin, const T * const searchMax, const T * const regionMin, const T * const regionMax) const {
			for (int d = 0; d < dim; d++) {
				if (LESS()(regionMin, searchMin, d)
					|| LESS()(searchMax, regionMax, d)) {
					return false;
				}
			}
			return true;
		}

		/// @brief checks wether two regions intersect or not.
		/// @returns true, if the search region and the other region intersect, false otherwise
		bool regionIntersects(const T * const searchMin, const T * const searchMax, const T * const regionMin, const T * const regionMax) const {
			for (int d = 0; d < dim; d++) {
				if (LESS()(regionMax, searchMin, d)
					|| LESS()(searchMax, regionMin, d)) {
					return false;
				}
			}
			return true;
		}

		/// @brief checks, wether node->value is within the search range.
		/// @returns true, if node->value[d] >= searchMin[d] and node->value[d] <= searchMax[d] for all dimensions d. otherwise false
		bool nodeInSearchRange(const T * const searchMin, const T * const searchMax, Node * const node) const {
			for (int d = 0; d < dim; d++) {
				if (LESS()(node->value, searchMin, d)
					|| LESS()(searchMax, node->value, d)) {
					return false;
				}
			}
			return true;
		}

		std::vector<T*> queryRectangle(Node * const node, T * const searchMin, T * const searchMax, T * const currentRegionMin, T * const currentRegionMax, const unsigned int depth) const {
			std::vector<T*> result;

			// if node->left == nullptr then also node->right == nullptr.
			// this means we reached a leaf of the tree
			if (node->left == nullptr) {
				// if the leaf is within the search range, add it to the result vector
				if (nodeInSearchRange(searchMin, searchMax, node)) {
					result.push_back(node->value);
				}
			}
			else {
				T *leftRegionMin = NEW()();
				T *leftRegionMax = NEW()();
				T *rightRegionMin = NEW()();
				T *rightRegionMax = NEW()();

				for (int i = 0; i < dim; i++) {
					SET()(leftRegionMin, currentRegionMin, i);
					SET()(leftRegionMax, currentRegionMax, i);
					SET()(rightRegionMin, currentRegionMin, i);
					SET()(rightRegionMax, currentRegionMax, i);
				}

				SET()(leftRegionMax, node->value, depth%dim);
				SET()(rightRegionMin, node->value, depth%dim);

				if (regionContained(searchMin, searchMax, leftRegionMin, leftRegionMax)) {
					reportSubtree(node->left, &result);
				}
				else {
					if (regionIntersects(searchMin, searchMax, leftRegionMin, leftRegionMax)) {
						std::vector<T*> resultLeft = queryRectangle(node->left, searchMin, searchMax, leftRegionMin, leftRegionMax, depth + 1);
						result.insert(result.end(), resultLeft.begin(), resultLeft.end());
					}
				}

				if (regionContained(searchMin, searchMax, rightRegionMin, rightRegionMax)) {
					reportSubtree(node->right, &result);
				}
				else {
					if (regionIntersects(searchMin, searchMax, rightRegionMin, rightRegionMax)) {
						std::vector<T*> resultRight = queryRectangle(node->right, searchMin, searchMax, rightRegionMin, rightRegionMax, depth + 1);
						result.insert(result.end(), resultRight.begin(), resultRight.end());
					}
				}

				DELETE()(leftRegionMin);
				DELETE()(leftRegionMax);
				DELETE()(rightRegionMin);
				DELETE()(rightRegionMax);
			}
			return result;
		}

	public:
		KDTree(std::vector<T*> values) {
			if (values.empty()) {
				root = nullptr;
			}
			else {
				for (int d = 0; d < dim; d++) {
					SET()(boundingBoxMin, values[0], d);
					SET()(boundingBoxMax, values[0], d);
				}
				root = buildKDTree(values);
			}
		}

		~KDTree() {
			DELETE()(boundingBoxMin);
			DELETE()(boundingBoxMax);
		}

		std::vector<T*> queryRectangle(T* searchMin, T* searchMax) {
			for (int d = 0; d < dim; d++) {
				if (LESS()(searchMax, searchMin, d)) {
					return std::vector<T*>();
				}
			}

			if (root == nullptr) {
				return std::vector<T*>();
			}

			const int zeroDepth = 0;
			return queryRectangle(root, searchMin, searchMax, boundingBoxMin, boundingBoxMax, zeroDepth);
		}
	};

}

#endif
