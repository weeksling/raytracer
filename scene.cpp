//
//  Framework for a raytracer
//  File: scene.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "scene.h"
#include "material.h"
#define NELEMS(x) (sizeof(x) / sizeof(x[0]))

Color Scene::trace(const Ray &ray)
{
    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t) {
            min_hit = hit;
            obj = objects[i];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    Material *material = obj->material;            //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector


    /****************************************************
    * This is where you should insert the color
    * calculation (Phong model).
    *
    * Given: material, hit, N, V, lights[]
    * Sought: color
    *
    * Hints: (see triple.h)
    *        Triple.dot(Vector) dot product
    *        Vector+Vector      vector sum
    *        Vector-Vector      vector difference
    *        Point-Point        yields vector
    *        Vector.normalize() normalizes vector, returns length
    *        double*Color        scales each color component (r,g,b)
    *        Color*Color        dito
    *        pow(a,b)           a to the power of b
    ****************************************************/

    Color I = Color(0,0,0);
    for (int i = 0; i<lights.size();i++){

        // isolate light and calculate vector to Light
        Light *light = lights[i];
        Vector L = light->position-hit;
        L.normalize(); // Normalize L

        // store intensities for materials
        double kd = material->kd;
        double ka = material->ka;
        double ks = material->ks;

        // define material and light colors
        Color Cl = light->color;
        Color Cr = material->color;


        // in shadow?
        Object *collide = NULL;
        Hit check(std::numeric_limits<double>::infinity(), Vector());
        Ray shadowRay = Ray(hit, L);
        //
        Point jiggle = shadowRay.at( pow(2.0,-32.0));
        shadowRay = Ray(jiggle, L);
        bool shadow=false;
        for (int j = 0; j<objects.size(); j++){
            Hit intersect = objects[j]->intersect(shadowRay);
            if (intersect.t<check.t){
                check = intersect;
                collide = objects[j];
            }
        }
        shadow = (!collide ? false:true);

        if (shadow){
            Color Ia = Cr*ka; // placeholder for soft shadow
            I+=Ia;
        } else {
            // diffuse light
            Color Id = Cr * max(0.0,L.dot(N));
            
            // specular Light calculation
            Vector R = -L+2*(L.dot(N))*N;
            double n = material->n;
            Color Is = Cl*pow(max(0.0,V.dot(R)),n);
            
            // Calculate ambient light
            Color Ia = Cr;

            // Add the 3 light intesities to color
            I += kd*Id + ks*Is + ka*Ia;
        }
    }
    return I;
}

void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x, h-1-y, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray);
            col.clamp();
            img(x,y) = col;
        }
    }
}

void Scene::addObject(Object *o)
{
    objects.push_back(o);
}

void Scene::addLight(Light *l)
{
    lights.push_back(l);
}

void Scene::setEye(Triple e)
{
    eye = e;
}
