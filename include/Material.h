#ifndef MATERIAL_H
#define MATERIAL_H

class HitResult;

class ReflectResult
{
public:
    Ray m_ray;
    Vector3D m_color;
};

class Material
{
public:
    Vector3D m_color;
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) = 0;
};


class Diffuse : public Material
{
public:
    Diffuse(const Vector3D& color)
    {
        m_color = color;
    };
    
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) override
    {
        ReflectResult res;
        //TODO: 4. generate one scattered ray and assign it to res.m_ray

        // Generate two random numbers in the range [0, 1)
        float u1 = random_float();
        float u2 = random_float();

        // Convert u1 to polar angle theta (0 to pi)
        float theta = acos(sqrt(1.0f - u1));

        // Convert u2 to azimuthal angle phi (0 to 2*pi)
        float phi = 2 * M_PI * u2;

        // Convert spherical coordinates to Cartesian coordinates (direction on hemisphere)
        float x = sin(theta) * cos(phi);
        float y = sin(theta) * sin(phi);
        float z = cos(theta);

        // Create the random direction vector
        Vector3D randomDir(x, y, z);

        // Ensure the scattered ray is on the same hemisphere as the normal. Reflect to the opposite hemisphere if the dot product is negative
        if (dot(randomDir, hit.m_hitNormal) < 0.0f) {
            randomDir = randomDir.operator-(); 
        }

        res.m_ray = Ray(hit.m_hitPos, randomDir);
        res.m_color = m_color;
        return res;
    }
};


class Specular : public Material
{
public:
    Specular(const Vector3D& color)
    {
        m_color = color;
    }
    
    virtual ReflectResult reflect(Ray& ray, HitResult& hit) override
    {
        ReflectResult res;
        //TODO: 5. generate one mirrored ray and assign it to res.m_ray
    
        // Incoming ray direction (normalized)
        Vector3D incomingDir = ray.direction();
        float magnitude = incomingDir.length();
        
        // Normalize the vector by dividing each component by the magnitude
        if (magnitude > 0.0f) {
            operator/(incomingDir, magnitude);
        }

        // Compute the reflection direction using the reflection formula
        Vector3D reflectedDir = incomingDir - 2.0f * dot(incomingDir, hit.m_hitNormal) * hit.m_hitNormal;

        // Create the reflected ray, starting from the hit point and in the reflected direction
        res.m_ray = Ray(hit.m_hitPos, reflectedDir);
        
        res.m_color = m_color;
        return res;
    }
};
#endif
