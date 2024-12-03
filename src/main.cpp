#include "Camera.h"
#include "World.h"

#include <iostream>
#include <fstream>

//for multithreading
#include <vector>
#include <algorithm>
#include <execution>
#include <chrono>

//NOTE: ALL TODO COMMENTS KEPT FOR EASY REFERENCE

void write_color_to_file(std::ostream &out, Vector3D pixel_color, int samples_per_pixel)
{
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;
    auto scale = 1.0 / samples_per_pixel;
    r = clamp(256 * sqrt(scale * r), 0, 255);
    g = clamp(256 * sqrt(scale * g), 0, 255);
    b = clamp(256 * sqrt(scale * b), 0, 255);
    out << int(r) << ' ' << int(g) << ' ' << int(b)<< '\n';
}

Vector3D ray_hit_color(Ray& r, World& world, int max_light_bounce_num)
{
    if (max_light_bounce_num <= 0)
        return Vector3D(0,0,0);
    
    HitResult hit = world.hit(r, 0.001, std::numeric_limits<float>::infinity());
    if (hit.m_isHit)
    {
        ReflectResult res = hit.m_hitMaterial->reflect(r, hit);
        return res.m_color * ray_hit_color(res.m_ray, world, max_light_bounce_num-1);
    }
    return Vector3D(1, 1, 1);
}



//although unnessecary, I multithreaded because I did not want to wait forever. Even with multithreading, this took 2-3 minutes!
void generate_results(World &world, std::string result_ppm_path) {
    int width =  768;
    int height = 540;
    float aspect_ratio = width / float(height);
    int rays_per_pixel = 100;
    const int max_light_bounce_num = 5;
    
    Vector3D eye(20,3,3);
    Vector3D target(0,0,0);
    Vector3D up(0,1,0);
    float fov = 20;//degree
    Camera camera(eye, target, up, fov, aspect_ratio);

    //TODO: 1. set your own path for output image
    std::vector<Vector3D> pixels(width * height);
    std::for_each(
        std::execution::par,
        std::begin(pixels),
        std::end(pixels),
        [&](Vector3D& v) {
            auto idx = std::distance(pixels.data(), &v);
            for (int s = 0; s < rays_per_pixel; ++s) {
                float col = ((idx % width) + random_float()) / (width - 1);
                float row = ((idx / width) + random_float()) / (height - 1);
                Ray r = camera.generate_ray(col, row);
                v += ray_hit_color(r, world, max_light_bounce_num);
            }
        }
    );


    std::cout << "Raytracing done! Writing to file..." << std::endl;
    std::ofstream fout(result_ppm_path);
    fout << "P3\n" << width << ' ' << height << "\n255\n";
    for (int j = height - 1; j >= 0; j--)
        for (int i = 0; i < width; i++)
            write_color_to_file(fout, pixels[j * width + i], rays_per_pixel);
    fout.flush();
    fout.close();
    std::cout << "ppm saved at " << result_ppm_path << std::endl;
}



int main()
{   
    //Measured how long this took (for fun really)

    // Get the starting time point
    auto start = std::chrono::high_resolution_clock::now();
    
    World world;
    
    //TODO: 6. uncomment one by one and render the following worlds
    world.generate_scene_one_diffuse();
    generate_results(world, "1mdiffuse.ppm");

    world.generate_scene_one_specular();
    generate_results(world, "1specular.ppm");

    world.generate_scene_multi_diffuse();
    generate_results(world, "mdiffuse.ppm");

    world.generate_scene_multi_specular();
    generate_results(world, "mspecular.ppm");

    world.generate_scene_all();
    generate_results(world, "all.ppm");


    // Get the ending time point
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> duration = end - start;

    // Convert the duration to minutes and seconds
    int minutes = static_cast<int>(duration.count()) / 60;
    double seconds = duration.count() - (minutes * 60);

    // Output the duration in minutes and seconds
    std::cout << "Time taken: " << minutes << " minutes " << seconds << " seconds" << std::endl;

}

