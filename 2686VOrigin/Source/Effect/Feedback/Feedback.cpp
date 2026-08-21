#include "./Feedback.h"

#include <cmath>

void Feedback::setFeedbackVector(std::array<float, 8>& fvec, float sub) {
	fvec[0] = 0.0f;

	for (int i = 1; i < 8; i++) {
		fvec[i] = std::pow(2.0f, (float)i - sub);
	}
}
