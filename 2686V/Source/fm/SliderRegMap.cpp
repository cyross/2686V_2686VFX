#include "SliderRegMap.h"

int SliderRegMap::count(juce::Slider *slider)
{
	return sliderRegMap.count(slider);
}
