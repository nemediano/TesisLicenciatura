#ifndef MATH_HELPERS_H_
#define MATH_HELPERS_H_

#include <glm/glm.hpp>

namespace math {
//! Helper functions to certain math operations.
/*!
    Since glm does almost everything these are mostly wrappers and conversions
*/
//! Angle conversion from degree to radians
float toRadians(const float& degree);
//! Angle conversion from radians to degree
float toDegree(const float& radians);
//! Color format conversion from a byte in [0, 255] to a normalized float in [0.0f, 1.0f]
float toFCol(const unsigned char& c);
//! Color format conversion from a normalized float in [0.0f, 1.0f] to a byte in [0, 255]
unsigned char toICol(const float& v);
//! Color format conversion from byte values in [0, 255] to a normalized float vector in [0.0f, 1.0f]
glm::vec3 toFCol(const unsigned char& r, const unsigned char& g, const unsigned char& b);
//! Color format conversion from byte values in [0, 255] to a normalized float vector in [0.0f, 1.0f]
glm::vec4 toFCol(const unsigned char& r, const unsigned char& g, const unsigned char& b,
    const unsigned char& a);
//! Contributed volume of the triangular face (winding order matters!) to accumulate for volume
//! (After accumulation you need to divide by 6 to get the actual volume)
float tethVolume(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
} // namespace math



#endif
