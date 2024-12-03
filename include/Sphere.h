#ifndef SPHERE_H
#define SPHERE_H

#include <memory>
#include <cmath>

using namespace std;
class Material;

class HitResult {
public:
    HitResult() { m_isHit = false; };
    bool m_isHit;
    Vector3D m_hitPos;
    Vector3D m_hitNormal;
    shared_ptr<Material> m_hitMaterial;
    float m_t;
};


class Sphere {
    
public:
    Sphere() {}
    Sphere(Vector3D center, float r, shared_ptr<Material> m)
    {
        m_center = center;
        m_radius = r;
        m_pMaterial = m;
    }
    HitResult hit(Ray& r, float min_t, float max_t);

    public:
    Vector3D m_center;
    float m_radius;
    shared_ptr<Material> m_pMaterial;
};



//test if ray hits this sphere within range min_t and max_t
HitResult Sphere::hit(Ray& ray, float min_t, float max_t) {
    HitResult hit_result;

    Vector3D oc = operator-(ray.m_origin, m_center);
    float doc = dot(ray.m_direction, oc);
    float discriminant = (doc * doc) - dot(ray.m_direction, ray.m_direction) * (dot(oc, oc) - m_radius * m_radius);

    if (discriminant > 0) {
        float sqrt_discriminant = sqrt(discriminant);
        
        //This is for the +- part, we try the - first, then + if that is not a hit
        float ray_time = (ray_time < min_t || ray_time > max_t) ? 
                        (-doc - sqrt_discriminant) :   // Closest hit
                        (-doc + sqrt_discriminant);    // Second root

        //if the time it takes for a hit is within the range of min and max, then we fill in and return hit result
        if (ray_time >= min_t && ray_time <= max_t) {
            hit_result.m_isHit = true;
            hit_result.m_t = ray_time;
            hit_result.m_hitPos = ray.at(ray_time);
            hit_result.m_hitNormal = operator/(operator-(hit_result.m_hitPos, m_center), m_radius);
            hit_result.m_hitMaterial = m_pMaterial;
            return hit_result;
        }
    }

    /* TODO: 2. compute ray hit information on the sphere
    // and fill in hit result
    hit_result.m_isHit = ...;
    hit_result.m_t = ...;
    hit_result.m_hitPos = ...;
    hit_result.m_hitNormal = ...;
    hit_result.m_hitMaterial = ...;
    */

    //only if the time for a hit is not within range
    hit_result.m_isHit = false;
    return hit_result;
}

#endif
