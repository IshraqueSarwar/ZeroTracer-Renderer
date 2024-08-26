#ifndef COLOR_H
# define COLOR_H


#include "interval.h"
#include "vec3.h"


using color = vec3;


inline double linear_to_gamma(double linear_component){
	if(linear_component>0){
		return sqrt(linear_component);
	}
	return 0;
}



Uint32 convert_to_ARGB8888(const color& pixel_color) {
  Uint32 full_opacity = 0xFF000000;
  return full_opacity |
    (static_cast<Uint32>(255.999*pixel_color.x()) << 16) |
    (static_cast<Uint32>(255.999*pixel_color.y()) << 8)  |
    (static_cast<Uint32>(255.999*pixel_color.z()));
}


void write_color(std::ostream& out, const color& pixel_color){
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	// translating the [0,1] ranged values to byte range [0, 255].
	static const interval intensity(0.000, 0.999);
	int rbyte = int(256*intensity.clamp(r));
	int gbyte = int(256*intensity.clamp(g));
	int bbyte = int(256*intensity.clamp(b));

	out<<rbyte<<' '<<gbyte<<' '<<bbyte<<'\n';
}


#endif