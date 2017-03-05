#ifndef __GUARD_COLOR3_H__
#define __GUARD_COLOR3_H__

class Color3 {
public:
    typedef double* iterator;
    typedef const double* const_iterator;

    double& r, &g, &b;

    Color3(double x, double y, double z);

    // Copy constructor
    Color3(const Color3& other);

    // Coordinate indexing
    double& operator[](int i) { return coordinates[i]; }
    const double& operator[](int i) const { return coordinates[i]; }

    // Iterator functions
    iterator begin() { return coordinates; }
    iterator end() { return coordinates + 3; }
    const_iterator begin() const { return coordinates; }
    const_iterator end() const { return coordinates + 3; }

    Color3& operator=(const Color3& other);

    bool operator==(const Color3& other) const;

private:
    double coordinates[3];
};


#endif
