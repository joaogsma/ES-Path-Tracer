#ifndef ES_PATH_TRACER__SHADING__COLOR3_H_
#define ES_PATH_TRACER__SHADING__COLOR3_H_

class Color3 {
public:
    typedef double* iterator;
    typedef const double* const_iterator;

    double& r, &g, &b;

    Color3(double val);
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
    bool operator!=(const Color3& other) const { return !operator==(other); }
    
    Color3 operator+(const Color3& other) const;
    Color3 operator-(const Color3& other) const;

    Color3 operator/(double scalar) const;

    void operator+=(const Color3& other);
    void operator-=(const Color3& other);
    void operator*=(double scalar);
    void operator/=(double scalar);

    static Color3 zero() { return Color3(0, 0, 0); }

private:
    double coordinates[3];
};

Color3 operator*(const Color3& a, const Color3& b);
Color3 operator*(const Color3& c, double scalar);
Color3 operator*(double scalar, const Color3& c);

typedef Color3 Radiance3;
typedef Color3 Irradiance3;

#endif
