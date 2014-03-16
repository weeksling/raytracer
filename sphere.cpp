//
//  Framework for a raytracer
//  File: sphere.cpp
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

#include "sphere.h"
#include <iostream>
#include <math.h>

/************************** Sphere **********************************/

Hit Sphere::intersect(const Ray &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    * 
    * Insert calculation of ray/sphere intersection here. 
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/
    Vector OC = ray.O-position;

    double a = ray.D.dot(ray.D);
    double b = 2*(ray.D.dot(OC));
    double c = OC.dot(OC)-pow(r,2);
    //
    double dis = (pow(b,2)-4*a*c);

    // if roots are complex, return no hit
    if (dis<0)
        return Hit::NO_HIT();

    // calculate the two hit points
    double t1 = (-b + sqrt(dis))/(2*a);
    double t2 = (-b - sqrt(dis))/(2*a);

    // check that roots are not negative & return minimum positive
    if (t1<0 && t2<0) return Hit::NO_HIT();
    double t = posmin(t1, t2);
    
    // calculate intersect location
    OC.x = ray.O.x + t*ray.D.x;
    OC.y = ray.O.y + t*ray.D.y;
    OC.z = ray.O.z + t*ray.D.z;

    // calculater normal and normalize it
    Vector N = (OC-position);
    N.normalize();
    return Hit(t,N);
}

double Sphere::posmin (double a, double b){
    if (a<0.0 && b>0.0)
        return b;
    else if (a>0.0 && b<0.0)
        return a;

    if (a<b)
        return a;
    else
        return b;
}