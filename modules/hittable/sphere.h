#ifndef SPHERE_H
#define SPHERE_H

class sphere : public hittable {
public:
    sphere(const point3& center, double radius) : center(center), radius(std::fmax(0, radius)) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        vec3 oc = center - r.origin();
        // variables for half_b variant of quadratic formula
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // the answer to the half_b quadratic formula
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        //root solution
        rec.t = root;
        //the point where the ray and the surface of the hittable intersects
        rec.p = r.at(rec.t);
        //calculate surface normal
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3 center;
    double radius;
};

#endif
