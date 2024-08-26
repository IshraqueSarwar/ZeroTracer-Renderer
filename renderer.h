#pragma once

#include <thread>
#include <atomic>

#include "camera.h"
#include "hittable.h"
#include "material.h"
#include "hittable_list.h"
#include "image.h"

struct video_params{
  int seconds;
  int fps;
  video_params(int s, int f) : seconds(s), fps(f) {} 
};

struct spinning_circle_params {
  vec3 e1;
  vec3 e2;
  point3 center;
  double radians;
  video_params vp;
};

class renderer {
  public:
    typedef std::atomic<bool> a_bool;
    typedef std::atomic<int> a_int;

    renderer();

    void render_to_file(const std::string filename) const;
    void render_to_window() const;
    void render_shifting_focus(point3 startpoint, point3 endpoint, const video_params& vp);
    void render_spinning_circle(const spinning_circle_params& scp);
    void render_straight_line(point3 endpoint, const video_params& vp);

  private:
    pixel ray_color(const ray& r, int depth, const hittable& world) const;
    void st_render_to_mem(image* const pixels, a_int& scanlines, a_bool* KILL) const;
    void mt_render_to_mem(image* const pixels, a_bool* RENDER_DONE, a_bool* KILL) const;
    int frame_count;

  public:  // perhaps make a bunch of these private and set them in the constructor
    hittable_list world;
    camera cam;
    int image_width;
    int image_height;
    int samples_per_pixel;
    int bounce_depth;
    int core_count;
};
