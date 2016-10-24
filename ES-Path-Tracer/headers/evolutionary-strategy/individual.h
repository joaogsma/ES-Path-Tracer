#ifndef __GUARD_INDIVIDUAL_H__
#define __GUARD_INDIVIDUAL_H__

#include <random>
#include <vector>

class Individual {
public:
	typedef std::vector<double>::size_type size_type;
	typedef std::vector<double>::iterator iterator;
	typedef std::vector<double>::const_iterator const_iterator;

	static const double COMP_EPSILON;

	Individual(int length);
	Individual(int length, std::mt19937 random_engine);

	// Iterator functions
	iterator begin() { return attributes.begin(); }
	iterator end() { return attributes.end(); }
	const_iterator begin() const { return attributes.begin(); }
	const_iterator end() const { return attributes.end(); }

	size_type size() const { return attributes.size(); }

	// Indexing operator
	double& operator[](size_type position) { return attributes[position]; }
	const double& operator[](size_type position) const { return attributes[position]; }

	Individual& operator=(const Individual& other);

	bool operator==(const Individual& other) const;

private:
	std::vector<double> attributes;
};

#endif // !__GUARD_INDIVIDUAL_H__

