#include "rtweekend.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"

using namespace std;


int main(){
	hittable_list world;
	camera cam;

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.4));
	auto material_center = make_shared<lambertian>(color(0.1, 0.1, 0.5));
	auto material_left   = make_shared<dielectric>(1.50);
	auto material_bubble = make_shared<dielectric>(1.00/1.50);
	auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.1);



	// world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	// world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
	// world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	// world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
	// world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	cam.world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	cam.world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, material_center));
	cam.world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	cam.world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_bubble));
	cam.world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	
	cam.aspect_ratio = 16.0/9.0;
	cam.image_width = 720;
	cam.image_height= cam.image_width / cam.aspect_ratio;
	cam.samples_per_pixel = 150;
	cam.max_depth = 50;

	// camera controls
	cam.vfov = 25;// the lower the value the greater the zoom.
	cam.lookfrom = point3(-2,2,1);
	cam.lookat = point3(0,0,-1);
	cam.vup = vec3(0,1,0);

	cam.defocus_angle = 1.0;
	cam.focus_dist = 3.4;

	// cam.world = world;

	cam.initialize();

	cam.render(cam.world);
	// cam.render_to_window();




    
    return 0;

}
