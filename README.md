# KDTree
A generic, multidimensional k-d tree written in c++11.

## Usage
Here we have three dimensional data of integer type. Put the data into a std::vector:
```
const int dim = 3;
int e1[dim] = {1, 1, 1};
int e2[dim] = {1, 1, 1};
int e3[dim] = {1, 2, 9};
int e4[dim] = {1, 2, 1};
int e5[dim] = {8, 8, 8};

std::vector<int*> values;
values.push_back(e1);
values.push_back(e2);
values.push_back(e3);
values.push_back(e4);
values.push_back(e5);
```
Build a three dimensional tree of integer type:
```
kdtree::KDTree<int, dim> tree(values);
```
Query all datapoints inside a rectangle:
```
int searchRectMin[5] = {0,0,0};
int searchRectMax[5] = {2,2,2};

std::vector<int*> result = tree.queryRectangle(searchRectMin, searchRectMax);
// result now contains e1, e2 and e4 
```

For advanced usage see [example2.cpp](example2.cpp).

## License
This software is licensed under the [MIT](LICENSE) license.
