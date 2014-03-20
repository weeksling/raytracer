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
#define AIR 1.000293

Color Scene::trace(const Ray &ray, int depth)
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
    N.normalize();

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
        Vector R = -L+2*(L.dot(N))*N;
        R.normalize(); // Normalize R
        L.normalize(); // Normalize L

        // store intensities for materials
        double kd = material->kd;
        double ka = material->ka;
        double ks = material->ks;
        double r = material->reflect;
        double rfr = material->refract;
        double eta = material->eta;

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
            Color Ia = Cl; // placeholder for soft shadow
            I+=Ia*ka;
        } else {
            // diffuse light
            Color Id = Cr * max(0.0,L.dot(N));
            
            // specular Light calculation
            
            double n = material->n;
            Color Is = Cl*pow(max(0.0,V.dot(R)),n);
            
            // Calculate ambient light
            Color Ia = Cl*Cr;

            // Calculate reflection of rays
            if (depth<5 && r>0){
                Hit refHit (std::numeric_limits<double>::infinity(), Vector());
                Object *refObj = NULL;
                Vector refDir = ray.D-2*(ray.D.dot(N))*N;
                Ray refRay(jiggle, refDir);
                for (int j = 0; j<objects.size(); j++){
                    Hit intersect = objects[j]->intersect(refRay);
                    if (intersect.t<refHit.t){
                        refHit = intersect;
                        refObj = objects[j];
                    }
                }

                if (refObj && refHit.t>0){
                    Is = Is+trace(refRay,depth+1)*r;
                }
            }


            // Calculate Refraction
            if (depth<5 && rfr>0.0){
                // define variables
                double eta1;
                double eta2;
                Vector t;

                // determine if ray is entering or leaving
                if(L.dot(N)>0){
                    eta1=AIR;
                    eta2=material->eta;
                } else if (L.dot(N)<0){
                    eta1=material->eta;
                    eta2=AIR;
                } else break;

                // calculate t direction vector
                double root = 1-pow(eta1,2)*(1-pow(ray.D.dot(N),2))/(pow(eta2,2));
                if (root<0) break;
                t = (eta1*(ray.D-N*(ray.D.dot(N)))/eta2)-N*(sqrt(root));
                t.normalize();

                Ray refractRay(jiggle, t);
                Is = Is + trace(refractRay, depth+1)*rfr;
            }

            // Add the 3 light intesities to color
            I += kd*Id + ks*Is + ka*Ia;
        }
        if(I.length()<0){
            return Color(0.0,0.0,0.0);
        }
        
    }
    return I/lights.size();
}


Color Scene::trace(const Ray &ray){
    return trace(ray, 0);
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
