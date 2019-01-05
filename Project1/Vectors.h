
#ifndef __VECTORS_H__
#define __VECTORS_H__

#include <iostream>


struct Vector2;
struct Vector3;
struct Vector4;

struct Vector2f;
struct Vector3f;
struct Vector4f;

typedef struct Vector2 Position2;
typedef struct Vector3 Position3;
typedef struct Vector4 Position4;

typedef struct Vector4f Color;

///////////////////////////////////////////////////////////////////////////////
// 2D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector2
{
	double x;
	double y;

	// ctors
	Vector2() : x(0), y(0) {};
	Vector2(double x, double y) : x(x), y(y) {};
	Vector2(const Vector2f& rhs);

	// utils functions
	void        set(double x, double y);
	double       length() const;                         //
	double       distance(const Vector2& vec) const;     // distance between two vectors
	Vector2&    normalize();                            //
	double       dot(const Vector2& vec) const;          // dot product
	double       cross(const Vector2& vec) const;          // cross product
	bool        equal(const Vector2& vec, double e) const; // compare with epsilon
	Vector2&		rotate(double radian);

	// operators
	Vector2     operator-() const;                      // unary operator (negate)
	Vector2     operator+(const Vector2& rhs) const;    // add rhs
	Vector2     operator-(const Vector2& rhs) const;    // subtract rhs
	Vector2&    operator+=(const Vector2& rhs);         // add rhs and update this object
	Vector2&    operator-=(const Vector2& rhs);         // subtract rhs and update this object
	Vector2     operator*(const double scale) const;     // scale
	Vector2     operator*(const Vector2& rhs) const;    // multiply each element
	Vector2&    operator*=(const double scale);          // scale and update this object
	Vector2&    operator*=(const Vector2& rhs);         // multiply each element and update this object
	Vector2     operator/(const double scale) const;     // inverse scale
	Vector2&    operator/=(const double scale);          // scale and update this object
	bool        operator==(const Vector2& rhs) const;   // exact compare, no epsilon
	bool        operator!=(const Vector2& rhs) const;   // exact compare, no epsilon
	bool        operator<(const Vector2& rhs) const;    // comparison for sort
	double       operator[](int index) const;            // subscript operator v[0], v[1]
	double&      operator[](int index);                  // subscript operator v[0], v[1]

	friend Vector2 operator*(const double a, const Vector2 vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector2& vec);
};



///////////////////////////////////////////////////////////////////////////////
// 3D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector3
{
	double x;
	double y;
	double z;

	// ctors
	Vector3() : x(0), y(0), z(0) {};
	Vector3(double x, double y, double z) : x(x), y(y), z(z) {};
	Vector3(const Vector3f& rhs);

	// utils functions
	void        set(double x, double y, double z);
	double       length() const;                         //
	double       distance(const Vector3& vec) const;     // distance between two vectors
	Vector3&    normalize();                            //
	double       dot(const Vector3& vec) const;          // dot product
	Vector3     cross(const Vector3& vec) const;        // cross product
	bool        equal(const Vector3& vec, double e) const; // compare with epsilon

															// operators
	Vector3     operator-() const;                      // unary operator (negate)
	Vector3     operator+(const Vector3& rhs) const;    // add rhs
	Vector3     operator-(const Vector3& rhs) const;    // subtract rhs
	Vector3&    operator+=(const Vector3& rhs);         // add rhs and update this object
	Vector3&    operator-=(const Vector3& rhs);         // subtract rhs and update this object
	Vector3     operator*(const double scale) const;     // scale
	Vector3     operator*(const Vector3& rhs) const;    // multiplay each element
	Vector3&    operator*=(const double scale);          // scale and update this object
	Vector3&    operator*=(const Vector3& rhs);         // product each element and update this object
	Vector3     operator/(const double scale) const;     // inverse scale
	Vector3&    operator/=(const double scale);          // scale and update this object
	bool        operator==(const Vector3& rhs) const;   // exact compare, no epsilon
	bool        operator!=(const Vector3& rhs) const;   // exact compare, no epsilon
	bool        operator<(const Vector3& rhs) const;    // comparison for sort
	double       operator[](int index) const;            // subscript operator v[0], v[1]
	double&      operator[](int index);                  // subscript operator v[0], v[1]

	friend Vector3 operator*(const double a, const Vector3 vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector3& vec);
};



///////////////////////////////////////////////////////////////////////////////
// 4D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector4
{
	double x;
	double y;
	double z;
	double w;

	// ctors
	Vector4() : x(0), y(0), z(0), w(0) {};
	Vector4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {};
	Vector4(const Vector4f& rhs);

	// utils functions
	void        set(double x, double y, double z, double w);
	double       length() const;                         //
	double       distance(const Vector4& vec) const;     // distance between two vectors
	Vector4&    normalize();                            //
	double       dot(const Vector4& vec) const;          // dot product
	bool        equal(const Vector4& vec, double e) const; // compare with epsilon

															// operators
	Vector4     operator-() const;                      // unary operator (negate)
	Vector4     operator+(const Vector4& rhs) const;    // add rhs
	Vector4     operator-(const Vector4& rhs) const;    // subtract rhs
	Vector4&    operator+=(const Vector4& rhs);         // add rhs and update this object
	Vector4&    operator-=(const Vector4& rhs);         // subtract rhs and update this object
	Vector4     operator*(const double scale) const;     // scale
	Vector4     operator*(const Vector4& rhs) const;    // multiply each element
	Vector4&    operator*=(const double scale);          // scale and update this object
	Vector4&    operator*=(const Vector4& rhs);         // multiply each element and update this object
	Vector4     operator/(const double scale) const;     // inverse scale
	Vector4&    operator/=(const double scale);          // scale and update this object
	bool        operator==(const Vector4& rhs) const;   // exact compare, no epsilon
	bool        operator!=(const Vector4& rhs) const;   // exact compare, no epsilon
	bool        operator<(const Vector4& rhs) const;    // comparison for sort
	double       operator[](int index) const;            // subscript operator v[0], v[1]
	double&      operator[](int index);                  // subscript operator v[0], v[1]

	friend Vector4 operator*(const double a, const Vector4 vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector4& vec);
};

///////////////////////////////////////////////////////////////////////////////
// 2D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector2f
{
    float x;
    float y;

    // ctors
	Vector2f() : x(0), y(0) {}
	Vector2f(float x, float y) : x(x), y(y) {}
	Vector2f(const Vector2& rhs);

    // utils functions
    void        set(float x, float y);
    float       length() const;                         //
    float       distance(const Vector2f& vec) const;     // distance between two vectors
	Vector2f&    normalize();                            //
	float       dot(const Vector2f& vec) const;          // dot product
	float       cross(const Vector2f& vec) const;          // cross product
	bool        equal(const Vector2f& vec, float e) const; // compare with epsilon
	Vector2f&     rotate(float radian); 

	// operators
	Vector2f     operator-() const;                      // unary operator (negate)
    Vector2f     operator+(const Vector2f& rhs) const;    // add rhs
    Vector2f     operator-(const Vector2f& rhs) const;    // subtract rhs
    Vector2f&    operator+=(const Vector2f& rhs);         // add rhs and update this object
    Vector2f&    operator-=(const Vector2f& rhs);         // subtract rhs and update this object
    Vector2f     operator*(const float scale) const;     // scale
    Vector2f     operator*(const Vector2f& rhs) const;    // multiply each element
    Vector2f&    operator*=(const float scale);          // scale and update this object
    Vector2f&    operator*=(const Vector2f& rhs);         // multiply each element and update this object
    Vector2f     operator/(const float scale) const;     // inverse scale
    Vector2f&    operator/=(const float scale);          // scale and update this object
    bool        operator==(const Vector2f& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector2f& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector2f& rhs) const;    // comparison for sort
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]

    friend Vector2f operator*(const float a, const Vector2f vec);
    friend std::ostream& operator<<(std::ostream& os, const Vector2f& vec);
};



///////////////////////////////////////////////////////////////////////////////
// 3D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector3f
{
    float x;
    float y;
    float z;

    // ctors
    Vector3f() : x(0), y(0), z(0) {}
    Vector3f(float x, float y, float z) : x(x), y(y), z(z) {}
	Vector3f(const Vector3& rhs);

    // utils functions
    void        set(float x, float y, float z);
    float       length() const;                         //
    float       distance(const Vector3f& vec) const;     // distance between two vectors
    Vector3f&    normalize();                            //
    float       dot(const Vector3f& vec) const;          // dot product
    Vector3f     cross(const Vector3f& vec) const;        // cross product
    bool        equal(const Vector3f& vec, float e) const; // compare with epsilon

	// operators
    Vector3f     operator-() const;                      // unary operator (negate)
    Vector3f     operator+(const Vector3f& rhs) const;    // add rhs
    Vector3f     operator-(const Vector3f& rhs) const;    // subtract rhs
    Vector3f&    operator+=(const Vector3f& rhs);         // add rhs and update this object
    Vector3f&    operator-=(const Vector3f& rhs);         // subtract rhs and update this object
    Vector3f     operator*(const float scale) const;     // scale
    Vector3f     operator*(const Vector3f& rhs) const;    // multiplay each element
    Vector3f&    operator*=(const float scale);          // scale and update this object
    Vector3f&    operator*=(const Vector3f& rhs);         // product each element and update this object
    Vector3f     operator/(const float scale) const;     // inverse scale
    Vector3f&    operator/=(const float scale);          // scale and update this object
    bool        operator==(const Vector3f& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector3f& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector3f& rhs) const;    // comparison for sort
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]

    friend Vector3f operator*(const float a, const Vector3f vec);
    friend std::ostream& operator<<(std::ostream& os, const Vector3f& vec);
};



///////////////////////////////////////////////////////////////////////////////
// 4D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector4f
{
    float x;
    float y;
    float z;
    float w;

    // ctors
	Vector4f() : x(0), y(0), z(0), w(0) {}
	Vector4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vector4f(const Vector4& rhs);

    // utils functions
    void        set(float x, float y, float z, float w);
    float       length() const;                         //
    float       distance(const Vector4f& vec) const;     // distance between two vectors
    Vector4f&    normalize();                            //
    float       dot(const Vector4f& vec) const;          // dot product
    bool        equal(const Vector4f& vec, float e) const; // compare with epsilon

	// operators
    Vector4f     operator-() const;                      // unary operator (negate)
    Vector4f     operator+(const Vector4f& rhs) const;    // add rhs
    Vector4f     operator-(const Vector4f& rhs) const;    // subtract rhs
    Vector4f&    operator+=(const Vector4f& rhs);         // add rhs and update this object
    Vector4f&    operator-=(const Vector4f& rhs);         // subtract rhs and update this object
    Vector4f     operator*(const float scale) const;     // scale
    Vector4f     operator*(const Vector4f& rhs) const;    // multiply each element
    Vector4f&    operator*=(const float scale);          // scale and update this object
    Vector4f&    operator*=(const Vector4f& rhs);         // multiply each element and update this object
    Vector4f     operator/(const float scale) const;     // inverse scale
    Vector4f&    operator/=(const float scale);          // scale and update this object
    bool        operator==(const Vector4f& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector4f& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector4f& rhs) const;    // comparison for sort
    float       operator[](int index) const;            // subscript operator v[0], v[1]
    float&      operator[](int index);                  // subscript operator v[0], v[1]

    friend Vector4f operator*(const float a, const Vector4f vec);
    friend std::ostream& operator<<(std::ostream& os, const Vector4f& vec);
};

///////////////////////////////////////////////////////////////////////////////
// 2D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector2n
{
	int x;
	int y;

	// ctors
	Vector2n() : x(0), y(0) {}
	Vector2n(int x, int y) : x(x), y(y) {}
	Vector2n(const Vector2& rhs);

	// utils functions
	void        set(int x, int y);
	int       dot(const Vector2n& vec) const;          // dot product
	int       cross(const Vector2n& vec) const;          // cross product

	// operators
	Vector2n     operator-() const;                      // unary operator (negate)
	Vector2n     operator+(const Vector2n& rhs) const;    // add rhs
	Vector2n     operator-(const Vector2n& rhs) const;    // subtract rhs
	Vector2n&    operator+=(const Vector2n& rhs);         // add rhs and update this object
	Vector2n&    operator-=(const Vector2n& rhs);         // subtract rhs and update this object
	Vector2n     operator*(const int scale) const;     // scale
	Vector2n     operator*(const Vector2n& rhs) const;    // multiply each element
	Vector2n&    operator*=(const int scale);          // scale and update this object
	Vector2n&    operator*=(const Vector2n& rhs);         // multiply each element and update this object
	Vector2n     operator/(const int scale) const;     // inverse scale
	Vector2n&    operator/=(const int scale);          // scale and update this object
	bool        operator==(const Vector2n& rhs) const;   // exact compare, no epsilon
	bool        operator!=(const Vector2n& rhs) const;   // exact compare, no epsilon
	bool        operator<(const Vector2n& rhs) const;    // comparison for sort
	int       operator[](int index) const;            // subscript operator v[0], v[1]
	int&      operator[](int index);                  // subscript operator v[0], v[1]

	friend Vector2n operator*(const int a, const Vector2n vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector2n& vec);
};



///////////////////////////////////////////////////////////////////////////////
// 3D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector3n
{
	int x;
	int y;
	int z;

	// ctors
	Vector3n() : x(0), y(0), z(0) {}
	Vector3n(int x, int y, int z) : x(x), y(y), z(z) {}
	Vector3n(const Vector3& rhs);

	// utils functions
	void        set(int x, int y, int z);
	int       dot(const Vector3n& vec) const;          // dot product
	Vector3n     cross(const Vector3n& vec) const;        // cross product

														   // operators
	Vector3n     operator-() const;                      // unary operator (negate)
	Vector3n     operator+(const Vector3n& rhs) const;    // add rhs
	Vector3n     operator-(const Vector3n& rhs) const;    // subtract rhs
	Vector3n&    operator+=(const Vector3n& rhs);         // add rhs and update this object
	Vector3n&    operator-=(const Vector3n& rhs);         // subtract rhs and update this object
	Vector3n     operator*(const int scale) const;     // scale
	Vector3n     operator*(const Vector3n& rhs) const;    // multiplay each element
	Vector3n&    operator*=(const int scale);          // scale and update this object
	Vector3n&    operator*=(const Vector3n& rhs);         // product each element and update this object
	Vector3n     operator/(const int scale) const;     // inverse scale
	Vector3n&    operator/=(const int scale);          // scale and update this object
	bool        operator==(const Vector3n& rhs) const;   // exact compare, no epsilon
	bool        operator!=(const Vector3n& rhs) const;   // exact compare, no epsilon
	bool        operator<(const Vector3n& rhs) const;    // comparison for sort
	int       operator[](int index) const;            // subscript operator v[0], v[1]
	int&      operator[](int index);                  // subscript operator v[0], v[1]

	friend Vector3n operator*(const int a, const Vector3n vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector3n& vec);
};



///////////////////////////////////////////////////////////////////////////////
// 4D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector4n
{
	int x;
	int y;
	int z;
	int w;

	// ctors
	Vector4n() : x(0), y(0), z(0), w(0) {}
	Vector4n(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) {}
	Vector4n(const Vector4& rhs);

	// utils functions
	void        set(int x, int y, int z, int w);
	int       dot(const Vector4n& vec) const;          // dot product

														   // operators
	Vector4n     operator-() const;                      // unary operator (negate)
	Vector4n     operator+(const Vector4n& rhs) const;    // add rhs
	Vector4n     operator-(const Vector4n& rhs) const;    // subtract rhs
	Vector4n&    operator+=(const Vector4n& rhs);         // add rhs and update this object
	Vector4n&    operator-=(const Vector4n& rhs);         // subtract rhs and update this object
	Vector4n     operator*(const int scale) const;     // scale
	Vector4n     operator*(const Vector4n& rhs) const;    // multiply each element
	Vector4n&    operator*=(const int scale);          // scale and update this object
	Vector4n&    operator*=(const Vector4n& rhs);         // multiply each element and update this object
	Vector4n     operator/(const int scale) const;     // inverse scale
	Vector4n&    operator/=(const int scale);          // scale and update this object
	bool        operator==(const Vector4n& rhs) const;   // exact compare, no epsilon
	bool        operator!=(const Vector4n& rhs) const;   // exact compare, no epsilon
	bool        operator<(const Vector4n& rhs) const;    // comparison for sort
	int       operator[](int index) const;            // subscript operator v[0], v[1]
	int&      operator[](int index);                  // subscript operator v[0], v[1]

	friend Vector4n operator*(const int a, const Vector4n vec);
	friend std::ostream& operator<<(std::ostream& os, const Vector4n& vec);
};


///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector2
///////////////////////////////////////////////////////////////////////////////
inline Vector2::Vector2(const Vector2f& rhs) : x(rhs.x), y(rhs.y) {}

inline Vector2 Vector2::operator-() const {
	return Vector2(-x, -y);
}

inline Vector2 Vector2::operator+(const Vector2& rhs) const {
	return Vector2(x + rhs.x, y + rhs.y);
}

inline Vector2 Vector2::operator-(const Vector2& rhs) const {
	return Vector2(x - rhs.x, y - rhs.y);
}

inline Vector2& Vector2::operator+=(const Vector2& rhs) {
	x += rhs.x; y += rhs.y; return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& rhs) {
	x -= rhs.x; y -= rhs.y; return *this;
}

inline Vector2 Vector2::operator*(const double a) const {
	return Vector2(x*a, y*a);
}

inline Vector2 Vector2::operator*(const Vector2& rhs) const {
	return Vector2(x*rhs.x, y*rhs.y);
}

inline Vector2& Vector2::operator*=(const double a) {
	x *= a; y *= a; return *this;
}

inline Vector2& Vector2::operator*=(const Vector2& rhs) {
	x *= rhs.x; y *= rhs.y; return *this;
}

inline Vector2 Vector2::operator/(const double a) const {
	return Vector2(x / a, y / a);
}

inline Vector2& Vector2::operator/=(const double a) {
	x /= a; y /= a; return *this;
}

inline bool Vector2::operator==(const Vector2& rhs) const {
	return (x == rhs.x) && (y == rhs.y);
}

inline bool Vector2::operator!=(const Vector2& rhs) const {
	return (x != rhs.x) || (y != rhs.y);
}

inline bool Vector2::operator<(const Vector2& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	return false;
}

inline double Vector2::operator[](int index) const {
	return (&x)[index];
}

inline double& Vector2::operator[](int index) {
	return (&x)[index];
}

inline void Vector2::set(double x, double y) {
	this->x = x; this->y = y;
}

inline double Vector2::length() const {
	return sqrt(x*x + y * y);
}

inline double Vector2::distance(const Vector2& vec) const {
	return sqrt((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y));
}

inline Vector2& Vector2::normalize() {
	const double EPSILON = 0.000001;
	double xxyy = x * x + y * y;
	if (xxyy < EPSILON)
		return *this;

	double invLength = 1.0f / sqrt(xxyy);
	x *= invLength;
	y *= invLength;
	return *this;
}

inline Vector2& Vector2::rotate(double radian) {
	double _x = x * cos(radian) - y * sin(radian);
	double _y = x * sin(radian) + y * cos(radian);
	x = _x; y = _y;
	return *this;
}

inline double Vector2::dot(const Vector2& rhs) const {
	return (x*rhs.x + y * rhs.y);
}

inline double Vector2::cross(const Vector2& rhs) const {
	return (x*rhs.y - y * rhs.x);
}
inline bool Vector2::equal(const Vector2& rhs, double epsilon) const {
	return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon;
}

inline Vector2 operator*(const double a, const Vector2 vec) {
	return Vector2(a*vec.x, a*vec.y);
}

inline std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
	os << "(" << vec.x << ", " << vec.y << ")";
	return os;
}
// END OF Vector2 /////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector3
///////////////////////////////////////////////////////////////////////////////
inline Vector3::Vector3(const Vector3f& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {};

inline Vector3 Vector3::operator-() const {
	return Vector3(-x, -y, -z);
}

inline Vector3 Vector3::operator+(const Vector3& rhs) const {
	return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

inline Vector3 Vector3::operator-(const Vector3& rhs) const {
	return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

inline Vector3& Vector3::operator+=(const Vector3& rhs) {
	x += rhs.x; y += rhs.y; z += rhs.z; return *this;
}

inline Vector3& Vector3::operator-=(const Vector3& rhs) {
	x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this;
}

inline Vector3 Vector3::operator*(const double a) const {
	return Vector3(x*a, y*a, z*a);
}

inline Vector3 Vector3::operator*(const Vector3& rhs) const {
	return Vector3(x*rhs.x, y*rhs.y, z*rhs.z);
}

inline Vector3& Vector3::operator*=(const double a) {
	x *= a; y *= a; z *= a; return *this;
}

inline Vector3& Vector3::operator*=(const Vector3& rhs) {
	x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this;
}

inline Vector3 Vector3::operator/(const double a) const {
	return Vector3(x / a, y / a, z / a);
}

inline Vector3& Vector3::operator/=(const double a) {
	x /= a; y /= a; z /= a; return *this;
}

inline bool Vector3::operator==(const Vector3& rhs) const {
	return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

inline bool Vector3::operator!=(const Vector3& rhs) const {
	return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

inline bool Vector3::operator<(const Vector3& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	if (z < rhs.z) return true;
	if (z > rhs.z) return false;
	return false;
}

inline double Vector3::operator[](int index) const {
	return (&x)[index];
}

inline double& Vector3::operator[](int index) {
	return (&x)[index];
}

inline void Vector3::set(double x, double y, double z) {
	this->x = x; this->y = y; this->z = z;
}

inline double Vector3::length() const {
	return sqrt(x*x + y * y + z * z);
}

inline double Vector3::distance(const Vector3& vec) const {
	return sqrt((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y) + (vec.z - z)*(vec.z - z));
}

inline Vector3& Vector3::normalize() {
	const double EPSILON = 0.000001;
	double xxyyzz = x * x + y * y + z * z;
	if (xxyyzz < EPSILON)
		return *this;

	double invLength = 1.0f / sqrt(xxyyzz);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return *this;
}

inline double Vector3::dot(const Vector3& rhs) const {
	return (x*rhs.x + y * rhs.y + z * rhs.z);
}

inline Vector3 Vector3::cross(const Vector3& rhs) const {
	return Vector3(y*rhs.z - z * rhs.y, z*rhs.x - x * rhs.z, x*rhs.y - y * rhs.x);
}

inline bool Vector3::equal(const Vector3& rhs, double epsilon) const {
	return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon && fabs(z - rhs.z) < epsilon;
}

inline Vector3 operator*(const double a, const Vector3 vec) {
	return Vector3(a*vec.x, a*vec.y, a*vec.z);
}

inline std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}
// END OF Vector3 /////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector4
///////////////////////////////////////////////////////////////////////////////
inline Vector4::Vector4(const Vector4f& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {};

inline Vector4 Vector4::operator-() const {
	return Vector4(-x, -y, -z, -w);
}

inline Vector4 Vector4::operator+(const Vector4& rhs) const {
	return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

inline Vector4 Vector4::operator-(const Vector4& rhs) const {
	return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

inline Vector4& Vector4::operator+=(const Vector4& rhs) {
	x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
}

inline Vector4& Vector4::operator-=(const Vector4& rhs) {
	x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
}

inline Vector4 Vector4::operator*(const double a) const {
	return Vector4(x*a, y*a, z*a, w*a);
}

inline Vector4 Vector4::operator*(const Vector4& rhs) const {
	return Vector4(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
}

inline Vector4& Vector4::operator*=(const double a) {
	x *= a; y *= a; z *= a; w *= a; return *this;
}

inline Vector4& Vector4::operator*=(const Vector4& rhs) {
	x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this;
}

inline Vector4 Vector4::operator/(const double a) const {
	return Vector4(x / a, y / a, z / a, w / a);
}

inline Vector4& Vector4::operator/=(const double a) {
	x /= a; y /= a; z /= a; w /= a; return *this;
}

inline bool Vector4::operator==(const Vector4& rhs) const {
	return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
}

inline bool Vector4::operator!=(const Vector4& rhs) const {
	return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
}

inline bool Vector4::operator<(const Vector4& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	if (z < rhs.z) return true;
	if (z > rhs.z) return false;
	if (w < rhs.w) return true;
	if (w > rhs.w) return false;
	return false;
}

inline double Vector4::operator[](int index) const {
	return (&x)[index];
}

inline double& Vector4::operator[](int index) {
	return (&x)[index];
}

inline void Vector4::set(double x, double y, double z, double w) {
	this->x = x; this->y = y; this->z = z; this->w = w;
}

inline double Vector4::length() const {
	return sqrt(x*x + y * y + z * z + w * w);
}

inline double Vector4::distance(const Vector4& vec) const {
	return sqrt((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y) + (vec.z - z)*(vec.z - z) + (vec.w - w)*(vec.w - w));
}

inline Vector4& Vector4::normalize() {
	//NOTE: leave w-component untouched
	const double EPSILON = 0.000001;
	double xxyyzz = x * x + y * y + z * z;
	if (xxyyzz < EPSILON)
		return *this;

	double invLength = 1.0f / sqrt(xxyyzz);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return *this;
}

inline double Vector4::dot(const Vector4& rhs) const {
	return (x*rhs.x + y * rhs.y + z * rhs.z + w * rhs.w);
}

inline bool Vector4::equal(const Vector4& rhs, double epsilon) const {
	return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon &&
		fabs(z - rhs.z) < epsilon && fabs(w - rhs.w) < epsilon;
}

inline Vector4 operator*(const double a, const Vector4 vec) {
	return Vector4(a*vec.x, a*vec.y, a*vec.z, a*vec.w);
}

inline std::ostream& operator<<(std::ostream& os, const Vector4& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
	return os;
}
// END OF Vector4 /////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector2f
///////////////////////////////////////////////////////////////////////////////
inline Vector2f::Vector2f(const Vector2& rhs) : x((float)rhs.x), y((float)rhs.y) {}

inline Vector2f Vector2f::operator-() const {
	return Vector2f(-x, -y);
}

inline Vector2f Vector2f::operator+(const Vector2f& rhs) const {
	return Vector2f(x + rhs.x, y + rhs.y);
}

inline Vector2f Vector2f::operator-(const Vector2f& rhs) const {
	return Vector2f(x - rhs.x, y - rhs.y);
}

inline Vector2f& Vector2f::operator+=(const Vector2f& rhs) {
	x += rhs.x; y += rhs.y; return *this;
}

inline Vector2f& Vector2f::operator-=(const Vector2f& rhs) {
	x -= rhs.x; y -= rhs.y; return *this;
}

inline Vector2f Vector2f::operator*(const float a) const {
	return Vector2f(x*a, y*a);
}

inline Vector2f Vector2f::operator*(const Vector2f& rhs) const {
	return Vector2f(x*rhs.x, y*rhs.y);
}

inline Vector2f& Vector2f::operator*=(const float a) {
	x *= a; y *= a; return *this;
}

inline Vector2f& Vector2f::operator*=(const Vector2f& rhs) {
	x *= rhs.x; y *= rhs.y; return *this;
}

inline Vector2f Vector2f::operator/(const float a) const {
	return Vector2f(x / a, y / a);
}

inline Vector2f& Vector2f::operator/=(const float a) {
	x /= a; y /= a; return *this;
}

inline bool Vector2f::operator==(const Vector2f& rhs) const {
	return (x == rhs.x) && (y == rhs.y);
}

inline bool Vector2f::operator!=(const Vector2f& rhs) const {
	return (x != rhs.x) || (y != rhs.y);
}

inline bool Vector2f::operator<(const Vector2f& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	return false;
}

inline float Vector2f::operator[](int index) const {
	return (&x)[index];
}

inline float& Vector2f::operator[](int index) {
	return (&x)[index];
}

inline void Vector2f::set(float x, float y) {
	this->x = x; this->y = y;
}

inline float Vector2f::length() const {
	return sqrtf(x*x + y * y);
}

inline float Vector2f::distance(const Vector2f& vec) const {
	return sqrtf((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y));
}

inline Vector2f& Vector2f::normalize() {
	const float EPSILON = 0.000001f;
	float xxyy = x * x + y * y;
	if (xxyy < EPSILON)
		return *this;

	float invLength = 1.0f / sqrtf(xxyy);
	x *= invLength;
	y *= invLength;
	return *this;
}
inline Vector2f& Vector2f::rotate(float radian) {
	float _x = x * cosf(radian) - y * sinf(radian);
	float _y = x * sinf(radian) + y * cosf(radian);
	x = _x; y = _y;
	return *this;
}

inline float Vector2f::dot(const Vector2f& vec) const {
	return (x*vec.x + y * vec.y);
}

inline float Vector2f::cross(const Vector2f& rhs) const {
	return (x*rhs.y - y * rhs.x);
}
inline bool Vector2f::equal(const Vector2f& rhs, float epsilon) const {
	return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon;
}

inline Vector2f operator*(const float a, const Vector2f vec) {
	return Vector2f(a*vec.x, a*vec.y);
}

inline std::ostream& operator<<(std::ostream& os, const Vector2f& vec) {
	os << "(" << vec.x << ", " << vec.y << ")";
	return os;
}
// END OF VECTOR2 /////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector3f
///////////////////////////////////////////////////////////////////////////////
inline Vector3f::Vector3f(const Vector3& rhs) : x((float)rhs.x), y((float)rhs.y), z((float)rhs.z) {}

inline Vector3f Vector3f::operator-() const {
	return Vector3f(-x, -y, -z);
}

inline Vector3f Vector3f::operator+(const Vector3f& rhs) const {
	return Vector3f(x + rhs.x, y + rhs.y, z + rhs.z);
}

inline Vector3f Vector3f::operator-(const Vector3f& rhs) const {
	return Vector3f(x - rhs.x, y - rhs.y, z - rhs.z);
}

inline Vector3f& Vector3f::operator+=(const Vector3f& rhs) {
	x += rhs.x; y += rhs.y; z += rhs.z; return *this;
}

inline Vector3f& Vector3f::operator-=(const Vector3f& rhs) {
	x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this;
}

inline Vector3f Vector3f::operator*(const float a) const {
	return Vector3f(x*a, y*a, z*a);
}

inline Vector3f Vector3f::operator*(const Vector3f& rhs) const {
	return Vector3f(x*rhs.x, y*rhs.y, z*rhs.z);
}

inline Vector3f& Vector3f::operator*=(const float a) {
	x *= a; y *= a; z *= a; return *this;
}

inline Vector3f& Vector3f::operator*=(const Vector3f& rhs) {
	x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this;
}

inline Vector3f Vector3f::operator/(const float a) const {
	return Vector3f(x / a, y / a, z / a);
}

inline Vector3f& Vector3f::operator/=(const float a) {
	x /= a; y /= a; z /= a; return *this;
}

inline bool Vector3f::operator==(const Vector3f& rhs) const {
	return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

inline bool Vector3f::operator!=(const Vector3f& rhs) const {
	return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

inline bool Vector3f::operator<(const Vector3f& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	if (z < rhs.z) return true;
	if (z > rhs.z) return false;
	return false;
}

inline float Vector3f::operator[](int index) const {
	return (&x)[index];
}

inline float& Vector3f::operator[](int index) {
	return (&x)[index];
}

inline void Vector3f::set(float x, float y, float z) {
	this->x = x; this->y = y; this->z = z;
}

inline float Vector3f::length() const {
	return sqrtf(x*x + y * y + z * z);
}

inline float Vector3f::distance(const Vector3f& vec) const {
	return sqrtf((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y) + (vec.z - z)*(vec.z - z));
}

inline Vector3f& Vector3f::normalize() {
	const float EPSILON = 0.000001f;
	float xxyyzz = x * x + y * y + z * z;
	if (xxyyzz < EPSILON)
		return *this;

	float invLength = 1.0f / sqrtf(xxyyzz);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return *this;
}

inline float Vector3f::dot(const Vector3f& rhs) const {
	return (x*rhs.x + y * rhs.y + z * rhs.z);
}

inline Vector3f Vector3f::cross(const Vector3f& rhs) const {
	return Vector3f(y*rhs.z - z * rhs.y, z*rhs.x - x * rhs.z, x*rhs.y - y * rhs.x);
}

inline bool Vector3f::equal(const Vector3f& rhs, float epsilon) const {
	return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon && fabs(z - rhs.z) < epsilon;
}

inline Vector3f operator*(const float a, const Vector3f vec) {
	return Vector3f(a*vec.x, a*vec.y, a*vec.z);
}

inline std::ostream& operator<<(std::ostream& os, const Vector3f& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}
// END OF Vector3f /////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector4f
///////////////////////////////////////////////////////////////////////////////
inline Vector4f::Vector4f(const Vector4& rhs) : x((float)rhs.x), y((float)rhs.y), z((float)rhs.z), w((float)rhs.w) {}

inline Vector4f Vector4f::operator-() const {
	return Vector4f(-x, -y, -z, -w);
}

inline Vector4f Vector4f::operator+(const Vector4f& rhs) const {
	return Vector4f(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

inline Vector4f Vector4f::operator-(const Vector4f& rhs) const {
	return Vector4f(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

inline Vector4f& Vector4f::operator+=(const Vector4f& rhs) {
	x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
}

inline Vector4f& Vector4f::operator-=(const Vector4f& rhs) {
	x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
}

inline Vector4f Vector4f::operator*(const float a) const {
	return Vector4f(x*a, y*a, z*a, w*a);
}

inline Vector4f Vector4f::operator*(const Vector4f& rhs) const {
	return Vector4f(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
}

inline Vector4f& Vector4f::operator*=(const float a) {
	x *= a; y *= a; z *= a; w *= a; return *this;
}

inline Vector4f& Vector4f::operator*=(const Vector4f& rhs) {
	x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this;
}

inline Vector4f Vector4f::operator/(const float a) const {
	return Vector4f(x / a, y / a, z / a, w / a);
}

inline Vector4f& Vector4f::operator/=(const float a) {
	x /= a; y /= a; z /= a; w /= a; return *this;
}

inline bool Vector4f::operator==(const Vector4f& rhs) const {
	return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
}

inline bool Vector4f::operator!=(const Vector4f& rhs) const {
	return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
}

inline bool Vector4f::operator<(const Vector4f& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	if (z < rhs.z) return true;
	if (z > rhs.z) return false;
	if (w < rhs.w) return true;
	if (w > rhs.w) return false;
	return false;
}

inline float Vector4f::operator[](int index) const {
	return (&x)[index];
}

inline float& Vector4f::operator[](int index) {
	return (&x)[index];
}

inline void Vector4f::set(float x, float y, float z, float w) {
	this->x = x; this->y = y; this->z = z; this->w = w;
}

inline float Vector4f::length() const {
	return sqrtf(x*x + y * y + z * z + w * w);
}

inline float Vector4f::distance(const Vector4f& vec) const {
	return sqrtf((vec.x - x)*(vec.x - x) + (vec.y - y)*(vec.y - y) + (vec.z - z)*(vec.z - z) + (vec.w - w)*(vec.w - w));
}

inline Vector4f& Vector4f::normalize() {
	//NOTE: leave w-component untouched
	const float EPSILON = 0.000001f;
	float xxyyzz = x * x + y * y + z * z;
	if (xxyyzz < EPSILON)
		return *this;

	float invLength = 1.0f / sqrtf(xxyyzz);
	x *= invLength;
	y *= invLength;
	z *= invLength;
	return *this;
}

inline float Vector4f::dot(const Vector4f& rhs) const {
	return (x*rhs.x + y * rhs.y + z * rhs.z + w * rhs.w);
}

inline bool Vector4f::equal(const Vector4f& rhs, float epsilon) const {
	return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon &&
		fabs(z - rhs.z) < epsilon && fabs(w - rhs.w) < epsilon;
}

inline Vector4f operator*(const float a, const Vector4f vec) {
	return Vector4f(a*vec.x, a*vec.y, a*vec.z, a*vec.w);
}

inline std::ostream& operator<<(std::ostream& os, const Vector4f& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
	return os;
}
// END OF Vector4f /////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector2n
///////////////////////////////////////////////////////////////////////////////
inline Vector2n::Vector2n(const Vector2& rhs) : x((int)rhs.x), y((int)rhs.y) {}

inline Vector2n Vector2n::operator-() const {
	return Vector2n(-x, -y);
}

inline Vector2n Vector2n::operator+(const Vector2n& rhs) const {
	return Vector2n(x + rhs.x, y + rhs.y);
}

inline Vector2n Vector2n::operator-(const Vector2n& rhs) const {
	return Vector2n(x - rhs.x, y - rhs.y);
}

inline Vector2n& Vector2n::operator+=(const Vector2n& rhs) {
	x += rhs.x; y += rhs.y; return *this;
}

inline Vector2n& Vector2n::operator-=(const Vector2n& rhs) {
	x -= rhs.x; y -= rhs.y; return *this;
}

inline Vector2n Vector2n::operator*(const int a) const {
	return Vector2n(x*a, y*a);
}

inline Vector2n Vector2n::operator*(const Vector2n& rhs) const {
	return Vector2n(x*rhs.x, y*rhs.y);
}

inline Vector2n& Vector2n::operator*=(const int a) {
	x *= a; y *= a; return *this;
}

inline Vector2n& Vector2n::operator*=(const Vector2n& rhs) {
	x *= rhs.x; y *= rhs.y; return *this;
}

inline Vector2n Vector2n::operator/(const int a) const {
	return Vector2n(x / a, y / a);
}

inline Vector2n& Vector2n::operator/=(const int a) {
	x /= a; y /= a; return *this;
}

inline bool Vector2n::operator==(const Vector2n& rhs) const {
	return (x == rhs.x) && (y == rhs.y);
}

inline bool Vector2n::operator!=(const Vector2n& rhs) const {
	return (x != rhs.x) || (y != rhs.y);
}

inline bool Vector2n::operator<(const Vector2n& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	return false;
}

inline int Vector2n::operator[](int index) const {
	return (&x)[index];
}

inline int& Vector2n::operator[](int index) {
	return (&x)[index];
}

inline void Vector2n::set(int x, int y) {
	this->x = x; this->y = y;
}

inline int Vector2n::dot(const Vector2n& vec) const {
	return (x*vec.x + y * vec.y);
}

inline int Vector2n::cross(const Vector2n& rhs) const {
	return (x*rhs.y - y * rhs.x);
}

inline Vector2n operator*(const int a, const Vector2n vec) {
	return Vector2n(a*vec.x, a*vec.y);
}

inline std::ostream& operator<<(std::ostream& os, const Vector2n& vec) {
	os << "(" << vec.x << ", " << vec.y << ")";
	return os;
}
// END OF VECTOR2 /////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector3n
///////////////////////////////////////////////////////////////////////////////
inline Vector3n::Vector3n(const Vector3& rhs) : x((int)rhs.x), y((int)rhs.y), z((int)rhs.z) {}

inline Vector3n Vector3n::operator-() const {
	return Vector3n(-x, -y, -z);
}

inline Vector3n Vector3n::operator+(const Vector3n& rhs) const {
	return Vector3n(x + rhs.x, y + rhs.y, z + rhs.z);
}

inline Vector3n Vector3n::operator-(const Vector3n& rhs) const {
	return Vector3n(x - rhs.x, y - rhs.y, z - rhs.z);
}

inline Vector3n& Vector3n::operator+=(const Vector3n& rhs) {
	x += rhs.x; y += rhs.y; z += rhs.z; return *this;
}

inline Vector3n& Vector3n::operator-=(const Vector3n& rhs) {
	x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this;
}

inline Vector3n Vector3n::operator*(const int a) const {
	return Vector3n(x*a, y*a, z*a);
}

inline Vector3n Vector3n::operator*(const Vector3n& rhs) const {
	return Vector3n(x*rhs.x, y*rhs.y, z*rhs.z);
}

inline Vector3n& Vector3n::operator*=(const int a) {
	x *= a; y *= a; z *= a; return *this;
}

inline Vector3n& Vector3n::operator*=(const Vector3n& rhs) {
	x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this;
}

inline Vector3n Vector3n::operator/(const int a) const {
	return Vector3n(x / a, y / a, z / a);
}

inline Vector3n& Vector3n::operator/=(const int a) {
	x /= a; y /= a; z /= a; return *this;
}

inline bool Vector3n::operator==(const Vector3n& rhs) const {
	return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

inline bool Vector3n::operator!=(const Vector3n& rhs) const {
	return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

inline bool Vector3n::operator<(const Vector3n& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	if (z < rhs.z) return true;
	if (z > rhs.z) return false;
	return false;
}

inline int Vector3n::operator[](int index) const {
	return (&x)[index];
}

inline int& Vector3n::operator[](int index) {
	return (&x)[index];
}

inline void Vector3n::set(int x, int y, int z) {
	this->x = x; this->y = y; this->z = z;
}

inline int Vector3n::dot(const Vector3n& rhs) const {
	return (x*rhs.x + y * rhs.y + z * rhs.z);
}

inline Vector3n Vector3n::cross(const Vector3n& rhs) const {
	return Vector3n(y*rhs.z - z * rhs.y, z*rhs.x - x * rhs.z, x*rhs.y - y * rhs.x);
}

inline Vector3n operator*(const int a, const Vector3n vec) {
	return Vector3n(a*vec.x, a*vec.y, a*vec.z);
}

inline std::ostream& operator<<(std::ostream& os, const Vector3n& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}
// END OF Vector3n /////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector4n
///////////////////////////////////////////////////////////////////////////////
inline Vector4n::Vector4n(const Vector4& rhs) : x((int)rhs.x), y((int)rhs.y), z((int)rhs.z), w((int)rhs.w) {}

inline Vector4n Vector4n::operator-() const {
	return Vector4n(-x, -y, -z, -w);
}

inline Vector4n Vector4n::operator+(const Vector4n& rhs) const {
	return Vector4n(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
}

inline Vector4n Vector4n::operator-(const Vector4n& rhs) const {
	return Vector4n(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
}

inline Vector4n& Vector4n::operator+=(const Vector4n& rhs) {
	x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
}

inline Vector4n& Vector4n::operator-=(const Vector4n& rhs) {
	x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
}

inline Vector4n Vector4n::operator*(const int a) const {
	return Vector4n(x*a, y*a, z*a, w*a);
}

inline Vector4n Vector4n::operator*(const Vector4n& rhs) const {
	return Vector4n(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
}

inline Vector4n& Vector4n::operator*=(const int a) {
	x *= a; y *= a; z *= a; w *= a; return *this;
}

inline Vector4n& Vector4n::operator*=(const Vector4n& rhs) {
	x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this;
}

inline Vector4n Vector4n::operator/(const int a) const {
	return Vector4n(x / a, y / a, z / a, w / a);
}

inline Vector4n& Vector4n::operator/=(const int a) {
	x /= a; y /= a; z /= a; w /= a; return *this;
}

inline bool Vector4n::operator==(const Vector4n& rhs) const {
	return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
}

inline bool Vector4n::operator!=(const Vector4n& rhs) const {
	return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
}

inline bool Vector4n::operator<(const Vector4n& rhs) const {
	if (x < rhs.x) return true;
	if (x > rhs.x) return false;
	if (y < rhs.y) return true;
	if (y > rhs.y) return false;
	if (z < rhs.z) return true;
	if (z > rhs.z) return false;
	if (w < rhs.w) return true;
	if (w > rhs.w) return false;
	return false;
}

inline int Vector4n::operator[](int index) const {
	return (&x)[index];
}

inline int& Vector4n::operator[](int index) {
	return (&x)[index];
}

inline void Vector4n::set(int x, int y, int z, int w) {
	this->x = x; this->y = y; this->z = z; this->w = w;
}

inline int Vector4n::dot(const Vector4n& rhs) const {
	return (x*rhs.x + y * rhs.y + z * rhs.z + w * rhs.w);
}

inline Vector4n operator*(const int a, const Vector4n vec) {
	return Vector4n(a*vec.x, a*vec.y, a*vec.z, a*vec.w);
}

inline std::ostream& operator<<(std::ostream& os, const Vector4n& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
	return os;
}
// END OF Vector4n /////////////////////////////////////////////////////////////



#endif
