#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>


#include <vector>
#include <thread>
#include <cmath>
#include <fstream>
#include <limits>
#include <float.h>
#include <chrono>
#include <cstdlib>

/*
=======================================
    SDL2
=======================================
*/

#include <SDL2/SDL.h>

using std::fabs;
using std::make_shared;
using std::shared_ptr;
using std::sqrt;



const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::duration<float>       duration;
typedef std::chrono::minutes               mins;
typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<float>> time_val;



void print_render_time(duration render_time_duration, std::ostream& out, int desired_precision) {
    mins  render_time_mins       = std::chrono::duration_cast<mins>(render_time_duration);
    float render_time_mins_float = float(render_time_mins.count());
    float render_time_secs_float = render_time_duration.count();
    float render_time_mins_secs  = render_time_mins_float + (render_time_secs_float - 60.0*render_time_mins_float)/60.0;

    int prev_precision = out.precision();
    out.precision(desired_precision);
    out << "\nRender complete.\nRender time: " << render_time_secs_float << "s, " << render_time_mins_secs << " mins"<<std::endl;
    out.precision(prev_precision);
}

inline double degree_to_radians(double degrees){
	return degrees * pi/180.0;
}


inline double random_double(){
	return rand() / (RAND_MAX+1.0);
}


inline double random_double(double min, double max){
	return min + (max-min)*random_double();
}



#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif