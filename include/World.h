#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <functional>
#include "Sphere.h"
#include "Material.h"

//using namespace std;

class World {
public:
    std::vector<shared_ptr<Sphere>> m_spheres;
    
    World() {}
    HitResult hit(Ray& ray, float min_t, float max_t);
    
    void generate_scene_one(Vector3D material_vector);
    void generate_scene_multi(shared_ptr<Material> sphere_material);

    void add_scene_floor();
    void generate_spheres(int row_start, int row_end,
                      int col_start, int col_end,
                      float spacing, float min_radius, float max_radius,
                      const std::function<std::shared_ptr<Material>()>& material_generator);

    
    void generate_scene_one_diffuse();
    void generate_scene_one_specular();
    void generate_scene_multi_diffuse();
    void generate_scene_multi_specular();
    void generate_scene_all();
};


HitResult World::hit(Ray& ray, float min_t, float max_t) {
    HitResult hit_result;
    
    // call sphere->hit(...)
    // hit_result should record the nearest hit
    for (const auto& curr_sphere : m_spheres) {
        HitResult hit = curr_sphere->hit(ray, min_t, max_t);
        if(hit.m_isHit) {
            hit_result = hit;
            max_t = hit_result.m_t;
        }
    }

    return hit_result;
}

// common re-used code that creates the floor in an image
void World::add_scene_floor() {
    auto material_floor = make_shared<Diffuse>(Vector3D(0.5, 0.5, 0.5));
    m_spheres.push_back(make_shared<Sphere>(Vector3D(0, -2000, 0), 2000, material_floor));
}

// generates the spheres in the image
void World::generate_spheres(int row_start, int row_end,
                             int col_start, int col_end,
                             float spacing, float min_radius, float max_radius,
                             const std::function<std::shared_ptr<Material>()>& material_generator) {

    for (int row = row_start; row < row_end; ++row) {
        for (int col = col_start; col < col_end; ++col) {
            float radius = random_float(min_radius, max_radius);
            Vector3D center(spacing * row + 0.5f * random_float(), radius,
                            spacing * col + 0.5f * random_float());

            m_spheres.push_back(make_shared<Sphere>(center, radius, material_generator()));
        }
    }
}

// generate just one sphere using diffuse or specular
void World::generate_scene_one(Vector3D material_vector) {
    m_spheres.clear();
    auto material = make_shared<Diffuse>(material_vector);
    m_spheres.push_back(make_shared<Sphere>(Vector3D(4, 1, 0), 1.0, material));
    add_scene_floor();
}

// generate multiple spheres using diffuse or specular
void World::generate_scene_multi(shared_ptr<Material> sphere_material) {
    m_spheres.clear();
    generate_spheres(-3, 3, -3, 3, 3.0f, 0.2f, 0.8f, [=]() { return sphere_material; });
    add_scene_floor();
}

// generate all spheres using diffuse and specular
void World::generate_scene_all() {
    m_spheres.clear();
    generate_spheres(-5, 10, -5, 5, 1.5f, 0.2f, 0.5f, []() -> std::shared_ptr<Material> {
        bool isDiffuse = random_float() <= 0.6f;
        Vector3D color = isDiffuse ? Vector3D::random() * Vector3D::random()
                                : Vector3D::random(0.5f, 1.0f);
         
        if (isDiffuse)
            return std::make_shared<Diffuse>(color);
        else
            return std::make_shared<Specular>(color);
    });
    add_scene_floor();
}


#endif
