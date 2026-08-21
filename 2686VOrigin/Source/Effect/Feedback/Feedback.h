#pragma once

#include <array>

class Feedback {
public:
	static void setFeedbackVector(std::array<float, 8>& fvec, float sub = 8.0f);
};