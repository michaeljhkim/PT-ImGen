#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Camera.h"
#include "World.h"

#include <iostream>
#include <fstream>

//for multithreading
#include <vector>
#include <algorithm>
#include <execution>
#include <chrono>

void write_color_to_file(std::ostream &out, Vector3D pixel_color, int samples_per_pixel) {
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

Vector3D ray_hit_color(Ray& r, World& world, int max_light_bounce_num) {
    if (max_light_bounce_num <= 0)
        return Vector3D(0,0,0);
    
    HitResult hit = world.hit(r, 0.001, std::numeric_limits<float>::infinity());
    if (hit.m_isHit) {
        ReflectResult res = hit.m_hitMaterial->reflect(r, hit);
        return res.m_color * ray_hit_color(res.m_ray, world, max_light_bounce_num-1);
    }
    return Vector3D(1, 1, 1);
}



// Multithreaded Rendering - Even with multithreading, this took 2-3 minutes!
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

    //set path for output image
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

    // Convert float RGB to 0–255 range and store in image_data - to allow stbi png write
    std::vector<unsigned char> image_data(width * height * 3); // 3 bytes per pixel (RGB)

    for (int y = 0; y < height; ++y) {
        int flipped_y = height - 1 - y; 

        for (int x = 0; x < width; ++x) {
            // flip the veritcal axis due to how stbi_image works
            int src_index = flipped_y * width + x;
            int dst_index = (y * width + x) * 3;

            Vector3D color = pixels[src_index] / float(rays_per_pixel);
            color.gamma_correction(2.0f);

            image_data[dst_index + 0] = static_cast<unsigned char>(color.x() * 255.0f);
            image_data[dst_index + 1] = static_cast<unsigned char>(color.y() * 255.0f);
            image_data[dst_index + 2] = static_cast<unsigned char>(color.z() * 255.0f);
        }
    }

    std::cout << "Raytracing done! Writing to file..." << std::endl;
    stbi_write_png(result_ppm_path.c_str(), width, height, 3, image_data.data(), width * 3);
    std::cout << "png saved at " << result_ppm_path << std::endl;
}


   
// Starts program
int main() {
    // Get the starting time point
    auto start = std::chrono::high_resolution_clock::now();
    
    World world;
    
    // one_diffuse
    world.generate_scene_one(Vector3D(0.3, 0.4, 0.5));
    generate_results(world, "1mdiffuse.png");

    // one_specular
    world.generate_scene_one(Vector3D(1, 1, 1));
    generate_results(world, "1specular.png");

    // multi_diffuse
    Vector3D diffuse_color = Vector3D::random() * Vector3D::random();
    world.generate_scene_multi( make_shared<Diffuse>(diffuse_color) );
    generate_results(world, "mdiffuse.png");

    // multi_specular
    Vector3D specular_color = Vector3D::random(0.3, 1);
    world.generate_scene_multi( make_shared<Specular>(specular_color) );
    generate_results(world, "mspecular.png");

    world.generate_scene_all();
    generate_results(world, "all.png");


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

