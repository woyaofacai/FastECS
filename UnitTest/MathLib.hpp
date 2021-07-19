/*
---------------------------------------------------------------------------
FastECS (Fast Entity-Component-System Library)
---------------------------------------------------------------------------

Original code by edisongao
contact: ed8266@gmail.com / 937907032@qq.com

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the FastECS team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the FastECS team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

#pragma once
#include <float.h>
#include <math.h>

template<class T>
inline bool fcmp_tpl(T a, T b, T fEpsilon = FLT_EPSILON)
{
	return fabs(a - b) <= fEpsilon;
}

template<class T>
class TVector3
{
public:
	const static int DIMENSION = 3;
	T x, y, z;

	TVector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	TVector3& operator=(const TVector3& t)
	{
		x = t.x;
		y = t.y;
		z = t.z;
		return *this;
	}

	TVector3(T x, T y, T z) :x(x), y(y), z(z) {}

	bool operator==(const TVector3& t) const {
		return x == t.x && y == t.y && z == t.z;
	}

	bool operator !=(const TVector3 &a) const {
		return x != a.x || y != a.y || z != a.z;
	}

	TVector3 operator-() const { return TVector3(-x, -y, -z); }

	TVector3 operator+(T k) const {
		return TVector3(x + k, y + k, z + k);
	}
	TVector3 operator+(const TVector3 &a) const {
		return TVector3(x + a.x, y + a.y, z + a.z);
	}

	TVector3 operator-(T k) const {
		return TVector3(x - k, y - k, z - k);
	}
	TVector3 operator-(const TVector3 &a) const {
		return TVector3(x - a.x, y - a.y, z - a.z);
	}

	TVector3 operator*(T k) const {
		return TVector3(x*k, y*k, z*k);
	}

	TVector3 operator *(const TVector3 &a) const {
		return TVector3(x * a.x, y * a.y, z * a.z);
	}

	TVector3 operator /(T a) const {
		auto oneOverA = recip_tpl(a); // NOTE: no check for divide by zero here
		return TVector3<T>(x*oneOverA, y*oneOverA, z*oneOverA);
	}

	TVector3& operator +=(const TVector3 &a) {
		x += a.x; y += a.y; z += a.z;
		return *this;
	}

	TVector3& operator +=(T k) {
		x += k; y += k; z += k;
		return *this;
	}

	TVector3& operator -=(const TVector3 &a) {
		x -= a.x; y -= a.y; z -= a.z;
		return *this;
	}

	TVector3& operator -=(T k) {
		x -= k; y -= k; z -= k;
		return *this;
	}

	TVector3& operator *=(T a) {
		x *= a; y *= a; z *= a;
		return *this;
	}

	TVector3& operator /=(T a) {
		auto oneOverA = recip_tpl(a);
		x *= oneOverA; y *= oneOverA; z *= oneOverA;
		return *this;
	}

	void Normalize() {
		T magSq = LengthSquared();
		if (magSq > 0)
		{
			// check for divide-by-zero
			T oneOverMag = recip_tpl(sqrt_tpl(magSq));
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
		}
	}

	auto Length() const {
		return (T)sqrt_tpl(x * x + y * y + z * z);
	}

	T LengthSquared() const {
		return x * x + y * y + z * z;
	}

	T Dot(const TVector3& a) const {
		return x * a.x + y * a.y + z * a.z;
	}

	TVector3 Cross(const TVector3 &b) const {
		return TVector3(
			y*b.z - z * b.y,
			z*b.x - x * b.z,
			x*b.y - y * b.x
		);
	}

	auto Distance(const TVector3 &b) const {
		T dx = x - b.x;
		T dy = y - b.y;
		T dz = z - b.z;
		return sqrt_tpl(dx * dx + dy * dy + dz * dz);
	}

	T DistanceSquared(const TVector3& b) const {
		T dx = x - b.x;
		T dy = y - b.y;
		T dz = z - b.z;
		return (dx*dx + dy * dy + dz * dz);
	}

	T DistanceSquared2D(const TVector3& b) const {
		T dx = x - b.x;
		T dy = y - b.y;
		return (dx * dx + dy * dy);
	}

	auto Distance2D(const TVector3& b) const {
		T dx = x - b.x;
		T dy = y - b.y;
		return sqrt_tpl(dx * dx + dy * dy);
	}
};

template<class T>
inline TVector3<T> Cross(const TVector3<T> &a, const TVector3<T> &b) {
	return TVector3<T>(
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x
		);
}

template<class T>
inline T Dot(const TVector3<T> &a, const TVector3<T> &b)
{
	return a.Dot(b);
}

template<>
inline bool TVector3<float>::operator==(const TVector3<float>& t) const {
	return fcmp_tpl(x, t.x) && fcmp_tpl(y, t.y) && fcmp_tpl(z, t.z);
}

template<>
inline bool TVector3<double>::operator==(const TVector3<double>& t) const {
	return fcmp_tpl(x, t.x) && fcmp_tpl(y, t.y) && fcmp_tpl(z, t.z);
}

template<>
inline bool TVector3<float>::operator!=(const TVector3<float>& t) const {
	return !fcmp_tpl(x, t.x) || !fcmp_tpl(y, t.y) || !fcmp_tpl(z, t.z);
}

template<>
inline bool TVector3<double>::operator!=(const TVector3<double>& t) const {
	return !fcmp_tpl(x, t.x) || !fcmp_tpl(y, t.y) || !fcmp_tpl(z, t.z);
}

template<class T>
inline TVector3<T> operator*(T k, const TVector3<T>& v)
{
	return TVector3<T>(v.x*k, v.y*k, v.z*k);
}


class Color {
public:

	float r, g, b, a;


	Color(float r = 0, float g = 0, float b = 0, float a = 0.0)
		: r(r), g(g), b(b), a(a) { }

	inline Color operator+(const Color& rhs) const {
		return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
	}

	inline Color& operator+=(const Color& rhs) {
		r += rhs.r; g += rhs.g; b += rhs.b; a += rhs.a;
		return *this;
	}

	inline Color operator*(const Color& rhs) const {
		return Color(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
	}

	inline Color& operator*=(const Color& rhs) {
		r *= rhs.r; g *= rhs.g; b *= rhs.b; a *= rhs.a;
		return *this;
	}

	inline Color operator*(float s) const {
		return Color(r * s, g * s, b * s, a * s);
	}

	inline Color& operator*=(float s) {
		r *= s; g *= s; b *= s; a *= s;
		return *this;
	}

	inline Color operator/(float s) const {
		float inv = 1.0f / s;
		return Color(r * inv, g * inv, b * inv, a * inv);
	}

	inline Color& operator/=(float s) {
		s = 1.0f / s;
		r *= s; g *= s; b *= s; a *= s;
		return *this;
	}

	inline bool operator==(const Color& rhs) const {
		return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
	}

	inline bool operator!=(const Color& rhs) const {
		return !operator==(rhs);
	}

	inline bool IsZero() const {
		return r == 0 && g == 0 && b == 0;
	}

	inline Color Clamp()
	{
		Color c = *this;
		if (c.r < 0) c.r = 0;
		if (c.g < 0) c.g = 0;
		if (c.b < 0) c.b = 0;
		if (c.a < 0) c.a = 0;
		return c;
	}

}; // class Color


inline Color operator*(float s, const Color& c) {
	return c * s;
}

using Vector3 = TVector3<float>;
using Vector3f = TVector3<float>;
using Vector3i = TVector3<int>;
using Vector3d = TVector3<double>;

