#ifndef CAMERA_H
#define CAMERA_H

// using namespace chrono_literals;
#include "rtweekend.h"
#include "hittable.h"
#include "material.h"
#include "hittable_list.h"
#include "image.h"

#include <thread>
#include <atomic>

#include <cstdlib>
#include <iostream>


using namespace std;
using namespace chrono_literals;

class camera{
public:
	typedef atomic<bool> a_bool;
    typedef atomic<int> a_int;

	double aspect_ratio = 16.0/9.0;
	int image_width = 500;
	int samples_per_pixel = 10;
	int max_depth = 10;



	double vfov = 90;
	point3 lookfrom = point3(0,0,0);
	point3 lookat = point3(0,0,-1);
	vec3 vup = vec3(0,1,0);


	double defocus_angle = 0;
	double focus_dist = 10;

	int image_height = image_width/aspect_ratio;
    int bounce_depth=50;
    int core_count=thread::hardware_concurrency();
    double aperature = 0.05;
    double lens_radius = aperature/2;



	point3 center;
	point3 pixel00_loc;
	vec3 pixel_delta_v;
	vec3 pixel_delta_u;
	double pixel_samples_scale;
	vec3 u,v,w;
	vec3 defocus_disk_u;
	vec3 defocus_disk_v;
	hittable_list world;

	void initialize(){
		image_height = int(image_width/aspect_ratio);
		image_height = (image_height<1) ? 1 : image_height;

		pixel_samples_scale = 1.0/samples_per_pixel;

		center = lookfrom;



		//INITIALIZING THE CAMERA
		auto theta = degree_to_radians(vfov);
		auto h = tan(theta/2);
		auto viewport_height = 2*h*focus_dist;
		auto viewport_width = viewport_height*(double(image_width)/ image_height);
		

		w = unit_vector(lookfrom-lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);


		//calculating the vectors across the horizonatal nd down the vertical viewport edges.
		vec3 viewport_u = viewport_width*u;
		vec3 viewport_v = viewport_height* -v;

		//calculating the horizontal and vertical delta vectors.
		pixel_delta_u = viewport_u/image_width;
		pixel_delta_v = viewport_v/image_height;

		// calculate the loc of upper left pixel
		auto viewport_upper_left = center - (focus_dist*w)-viewport_u/2-viewport_v/2;
		pixel00_loc = viewport_upper_left + 0.5 *(pixel_delta_u + pixel_delta_v);

		//calculating the camera defocus disk basis vec
		auto defocus_radius = focus_dist*tan(degree_to_radians(defocus_angle/2));
		defocus_disk_u = u*defocus_radius;
		defocus_disk_v = v*defocus_radius;
	}


	ray get_ray(double i, double j) const{
		auto offset = sample_square();
		auto pixel_sample = pixel00_loc+((i+offset.x()) * pixel_delta_u)+ ((j+offset.y()) * pixel_delta_v);

		auto ray_origin = (defocus_angle<=0)?center:defocus_disk_sample();
		auto ray_direction = pixel_sample-ray_origin;
		
		return ray(ray_origin, ray_direction);
	}


	// ray get_ray_mt(double u, double v){
	// 	vec3 rd = origin + lens_radius*random_in_unit_disk_mt();
	// 	return ray(rd, (top_left_corner + u*horizontal - v*vertical) - rd);
	// }

	vec3 sample_square()const{
		return vec3(random_double()-0.5, random_double()-0.5, 0);
	}

	point3 defocus_disk_sample() const{
		auto p = random_in_unit_disk();
		return center+(p[0]*defocus_disk_u)+(p[1]*defocus_disk_v);


	}



	pixel ray_color(const ray& r, int depth, const hittable& world)const{
		if(depth<=0){
			return color(0,0,0);
		}

		hit_record rec;
		if(world.hit(r, interval(0.001, infinity), rec)){
			ray scattered;
			pixel attenuation;
			if(rec.mat->scatter(r, rec, attenuation, scattered)){
				return attenuation* ray_color(scattered, depth-1, world);
			}
			return pixel(0,0,0);

		}

		vec3 unit_direction = unit_vector(r.direction());
		auto a = 0.5*(unit_direction.y() + 1.0);
		return (1.0-a)*pixel(1.0,1.0,1.0)+a*pixel(0.5,0.7,1.0);
	}



/* Single-threaded render to memory location passed in. Adds final pixel divided by core count to each output pixel. */
void st_render_to_mem(image* const pixels, a_int& scanlines, a_bool* KILL) const {

    // Split the render across all cores
    int divided_spp = samples_per_pixel/core_count;

    for (int i = 0; i < image_height; ++i) {
        for (int j = 0; j < image_width; ++j) {
            if (KILL != nullptr) if (*KILL == true) goto done;
            color sum;
            for (int k = 0; k < divided_spp; ++k) {
                double u = (j+random_double()) / image_width;
                double v = (i+random_double()) / image_height;
                
                ray r = get_ray(j, i);
                sum += ray_color(r, bounce_depth, world);
            }

   //          for(int sample = 0;sample< samples_per_pixel;sample++){
			// 		ray r = get_ray(j, i);
			// 		sum+=ray_color(r, max_depth, world);
			// }
            
            pixel final = (sum/divided_spp);   // sqrt for gamma correction
            pixel partial_avg = final / core_count;
            // cout<<partial_avg.x()<<'\n';
            (*pixels)(j,i) += convert_to_ARGB8888(partial_avg);
        }
        ++scanlines;
    }
    done: {};
}

/* Multi-threaded render to memory location passed in */
void mt_render_to_mem(image* const pixels, a_bool* RENDER_DONE, a_bool* KILL) const {

    // create an array of threads
    std::thread threads[core_count];

    // stores (# of scanlines completed * core_count); thus, divide by core_count to get # of scanlines done rendering
    a_int scanlines = 0;

    // launch as many threads as CPU cores, rendering one image on each thread
    for (int i = 0; i < core_count; ++i)
        threads[i] = std::thread(&camera::st_render_to_mem, this, pixels, std::ref(scanlines), KILL);

    // Print out rendering progress as a percentage
    while((scanlines/core_count) != image_height) {
        if (KILL != nullptr) if (*KILL) break; // Stop printing progress if KILL command has been issued
        std::cout << "\rProgress: " << std::ceil(((scanlines/core_count) / (double) image_height)*100.0) << "%" << std::flush;
    }

    // Wait for all threads to finish their renders
    for (int i = 0; i < core_count; ++i)
        threads[i].join();


    if (RENDER_DONE != nullptr && KILL != nullptr) {
        if (*KILL != true) { // We only want to add delay after scene has finished rendering; if it has not (window close command issued), do not add delay
            std::this_thread::sleep_for(80ms); // wit a small amount to allow final pixels to get drawn onto screen
            *RENDER_DONE = true;
        } else
            std::cout << "Render aborted. Exiting." << std::endl;
    }
}


/* Renders scene and shows it in a program window */
void render_to_window() const {
	
    // Print render info
    std::cout << "Scene render into desktop window started." << std::endl;
    std::cout << "Dimensions: " << image_width << " x " << image_height << std::endl;

    // Create window and renderer
    SDL_Init( SDL_INIT_EVERYTHING );
    SDL_Window* window = SDL_CreateWindow( "zeroTracer-Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, image_width, image_height, SDL_WINDOW_SHOWN);
    SDL_Renderer* sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );  // use linear filtering for scaling

    // Create texture and allocate space in memory for image
    SDL_Texture* texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, image_width, image_height);
    image* pixels = new image(image_width, image_height);  

    // Launch scene render on a separate thread
    a_bool RENDER_DONE = false;  // flag for stopping rendering pixels from memory to screen once render is finished
    a_bool KILL = false;  // flag for making the rendering threads stop and return; needed for when user closes the rendering window while render is still in progress
    auto start_time = Time::now();
    std::thread headless_render(&camera::mt_render_to_mem, this, pixels, &RENDER_DONE, &KILL);

    // Main SDL window loop
    bool running = true;
    bool timer_done = false;
    while (running) {
        // Print render time when render is finished
        if (RENDER_DONE && !timer_done) {
            print_render_time(Time::now() - start_time, std::cout, 3);
            timer_done = true;
        }

        // SDL event handling
        SDL_Event e;
        while(SDL_PollEvent(&e)) { // return 1 if there is a pending event, otherwise 0 (loop doesn't run)
            if (e.type == SDL_QUIT) {
                if (RENDER_DONE == false) {
                    KILL = true;  // send kill command to threads only if render is still in progress
                    std::cout << std::endl;  // make space for next render info on screen
                }
                running = false;
                break;
            }
        }

        if (RENDER_DONE == false && running == true) {  // Only continue copying image from memory into texture if render is in progress
            // Copy pixels from memory into the SDL texture
            Uint32* locked_pixels = nullptr;
            int pitch = image_width*4;
            SDL_LockTexture( texture, nullptr, reinterpret_cast<void**>(&locked_pixels), &pitch );

            for (int i = 0; i < image_width*image_height; ++i)
                locked_pixels[i] = (*pixels)[i];

            SDL_UnlockTexture(texture);

            // Copy texture to renderer
            SDL_RenderCopy(sdl_renderer, texture, nullptr, nullptr);
            // Update screen
            SDL_RenderPresent(sdl_renderer);
        }
    }

    // Join rendering thread
    headless_render.join();

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << std::endl;  // make space for next render info on screen

    delete pixels;
}


	void render(const hittable& world){
		// initialize();
		cout<<"P3\n"<<image_width<<' '<<image_height<<"\n255\n";
		for(int h = 0;h<image_height;h++){
			clog<<"\rScanlines remaining: "<<(image_height-h)<<' '<<flush;
			for(int w = 0;w<image_width;w++){
				color pixel_color(0,0,0);
				for(int sample = 0;sample< samples_per_pixel;sample++){
					ray r = get_ray(w, h);
					pixel_color+=ray_color(r, max_depth, world);
				}
				write_color(cout, pixel_samples_scale*pixel_color);
			}
		}

		clog<<"\rDone.				\n";
	}



// private:
	// int image_height;

	
};

#endif