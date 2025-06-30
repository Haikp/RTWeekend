In my first read through of this pdf, I had strictly taken a look at the cpp syntax to get a feel for how I should code. Doing so definitely gave me a better understanding of how cpp can be structured, and I made a small project doing so [here](https://github.com/Haikp/InvoiceTracker), making an invoice tracker. In my second read through, I took notes on the math behind ray tracing, located [here](https://github.com/Haikp/RTWeekend/blob/main/SecondReadThroughNotes.md).

#### 3.1 Color Utility Functions
A very cool trick to avoid floating point precision errors. Understanding that we know rgb can be potentially between 0 and 1, there may be an off chance that it could result in a value thats is 1. doing floating point arithmetic does not always result in the value we see on screen, which there may be a chance of it being 1.0001 and the like. We the product to result in a discrete value between 0 and 255.

As I'm typing this, I'm more or less realizing that the floating point precision is more of a biproduct of the true intent. I believe we use 255.99 because we would like 255 to be a possible value to obtain. If we used strictly 255, then the only way of returning a 255, would be to have a value of 1 on one of the RGB values. Any value between 0 and 0.999 would result in 0, while if we limited to 255, only 255 itself would result in a 255 when truncated. Using 255.999 as opposed to 256 however is for the floating point precision error, as using 256 allows the chance for a 256 to happen if the float checks out to be 1 or greater.

#### 4.2 Sending Rays Into the Scene
A very good image came to me while rereading this section. A viewport is simply the close side of the frustum for a camera. All rays, starting from the camera, will pass through this box. The number of rays is not dependent of the size of the viewport, but rather the resolution of the screen you're rendering. As such, messing with the view port is essentially tampering with only the direction that the ray points. Imagine a bunch of straws loosely bunched up on one hand, and use your other hand and form a ring which all straws pass through. The hand with the newly formed ring is the viewport, and the straws are the rays. Making this ring smaller or further away would mess with the rays, either bunching them all up together, or allowing them to freely spread out.

When we calculate the aspect ratio of our viewport, we should keep in mind to recalculate the aspect ratios based on our calculations if we were to derive other numbers that are clamped. Just be aware of the size of the viewport you're rendering.

When calculating where a ray is pointing, it is important to note that we want the ray to point towards the center of the pixel for more complex reasons other than clarity for which ray is aligned to which pixel. When looking at more complex techniques like anti-aliasing, we will shoot more than one ray at a specific pixel, to which we add a certain offset every time. This allows for the ray pick up samples that are relevant to that particular pixel. Also, generally, moving a ray by .5 of a pixel would probably influence what it hits depending on how far out the object it is.

To attempt to sum up the math of calculating where the ray should point to, your viewport is your window size, and the image width and height is the resolution you want. You need to divvy up the viewport such that the resolution fits the way you want it to. To reiterate, the resolution of your render is the number of rays being sent through the viewport. We want the rays the be evenly spaced in this case, as such we will divide the corresponding height and width of the viewport by the image resolution. This grants us our delta, or the distance between each ray for both x and y axis.

For this format, we will start from the top left of the screen, meaning we will need to get the top left pixels' coordinates. Starting from the camera, head to the appropriate xy-plane inline with the viewport, and simply subtract by half the length and width of the viewport. As mentioned previously, we want to aim for the center of the pixel, as such add by half of a pixel on both x and y axis.

Just want to make a note about "shooting rays", it feels more appropriate to envision it as checking for whether or not a pixel lands inside the calculation of the surface of a shape, either the surface of a 3D object, or the area of a shape. It's not real time shooting, just calculating between all objects that exists in the world, and seeing if it mathematically intersects with a shape. If it doesn't, it gets defaulted to a color. More on this later, but this just made it easier to understand what I'm working with.

On a simpler note, gradients are pretty easy to calculate. For example, (20%) blue + (80%) green. Shift the percentages depending on a variable you want, as long as they add up to 100%.

#### 5.1 Ray-Sphere Intersection
Reading the math is a little painful. But at the end of the day surely its just pattern recognition and a little math intuition. The goal here is to put as much of the equation as you can into vec3s, what ever method possible. In the case of a sphere, we were able to nicely format it into a dot product, recognizing the sum of products. I honestly don't know if I can do this on my own at the moment, this seems like a rather large jump, but I digress. As we are calculating if we are having contact with the surface of an object, we want to use the formula for a sphere, and see if it equates to the radius squared. To note as to what we are solving for: does at any point our ray come in contact with the spheres' surface. To determine our constants, we have the origin of the sphere, and the radius of the sphere. Our one variable is to see if there exists a point on the current ray that satisfies the equation. The equation for a ray is the `origin + (vector direction) * t`, where `t` is what we will be solving for. From here, the textbook does a good job of simplifying the equation such that it fits into the quadratic formula, its better to look there. Note, distributive properly for dot product behaves just like multiplication.

We now use the discriminant from quadratic formula to see if there are any intersections between the ray and the circle. If the discriminant is less than 0, then theres no solution, equal to 0 means 1 solution, and greater than 0 is 2 solutions.  The solutions represents the number of intersections.

#### 6.1 Shading with Surface Normals
Given the last section, we should already have an understanding of how we can identify if we have hit an object in world. Based off of that, we need to the calculate the normal of the space we hit. This will vary depending on the shape you are working with. Spheres are straight forward, as once a ray has made contact with a sphere, we are aware as to what coordinate the ray made contact with the sphere. We can use this point of contact, and subtract the origin of the circle from it, granting us the ray that points from the center of the sphere, outwards pointing to the point of contact, being our normal. These normals are unit vectors, and as such the length of a normal is a unit of 1, which also means the values of the vec3 of these normals range between -1 to 1. In order to make it usable as a color, we must omit the negative values by adding 1 to move the range to 0 to 2, then multiplying by .5 to bring it to 0 to 1.

#### 6.2 Half_b Version of the Quadratic Formula
Simplifying the quadratic formula, and makes it look nicer. It saves one multiplication, which is probably more useful than I think when considering the number of times this code is ran. There is a -2 that appears when calculating the values that go into the quadratic formula, and using the formula requires us to divide by -2.

See the math for yourself on the textbook.

#### 6.3 An Abstraction for Hittable Objects
The initial proposed solution in creating a list of hittable objects in world was to create an array of a shape and push back as many as desired. The problem with this is that you would need a list for each shape type then, which is not necessarily ideal.

The solution used is thus an abstract class that defines the bare minimum of a shape, and we create a class that holds a list of shared pointers that point to derived classes of the abstract class.

From this point, I believe its worth trying to dissect the design philosophy behind their objects.

`hittable.h` contains two classes: a virtual base class `hittable` and a data structure `hit_record`. The `hittable` class defines a common interface for all hittable objects, allowing them to be derived from a single base type. This abstraction enables cleaner and more modular code, especially when managing a scene composed of various types of objects.

The `hit_record` class encapsulates the information about a ray-object intersection, such as the hit point, normal, material, and whether the hit was on the front face. While these could theoretically be made part of `hittable`, separating them serves several important purposes.

Most notably, it allows the `hit()` function in `hittable` to be marked as `const`, meaning it promises not to modify the state of the object. If the hit information were stored directly within the `hittable` class, `hit()` would need to modify the object to update those values, breaking `const` correctness. Additionally, keeping `hit_record` separate ensures that derived classes can have a private state that remains unmodified during intersection tests.

Another benefit of `hit_record` being a standalone class is memory efficiency and simplicity in recursive ray tracing. Each ray path (including reflections and refractions) reuses the same `hit_record` instance during its computation, avoiding unnecessary memory allocations and making the system more cache-friendly.

#### 6.4 Front Faces Versus Back Faces
There are two methods of approach we can take this problem. Whether or not we want the normal to always point outwards, or if we want the normal to always point against the ray being cast.

In the case of this text for now, we will be using the case that normals will always be pointing outwards. Since we have a normal that points outwards, we are able to use the dot product to determine if the ray is inside the sphere or not. Image a unit circle, if the angle created is an acute angle, the cosine is positive, and if the angle is obtuse, then the cosine is negative. Vividly imagine the normal of a sphere that always points outwards. In attempt to create an acute angle with this restriction, the ray would have to point up and outwards as well. This would mean that the ray would be pointing towards the back face. This all leads to going back to the unit circle, where an acute angle would result in a positive value, meaning if the dot product results in a positive value, it means that its an acute angle, which goes back to it meaning the ray is pointing towards the backface. Using this logic, we can calculate the dot product, and see that if the value return is positive, then its pointing towards the back face, otherwise its pointing towards the front face.

Something to keep in mind, when determining the dot product, imagine as if the ray and the outward normal are now connected through its starting point. The angle created by this image is the angle you are judging. I do think that referring back to the unit circle is easier though and probably more relevant to remember.

In the case of the normal is always pointing towards the ray, then we simply need to store the outward normal as its own variable to calculate front or back face.

#### 6.8 An Interval Class
The interval class in itself is very straight forward, what I wanted to bring to light was how it was used to determine the closest object and what it should be rendering.

When determining what a ray has hit, we must pass through a declared interval. We are to set the max value of interval as the farthest distance, and loop through all objects to find the shortest distance. This is where polymorphism shines, allowing us to loop through and call hit on every individual object that has its own unique function.

Keep in mind that when determining which object is the closest, it must sort through all the objects it hit. This is not a real-time render where it is able to come in contact with the first object, but rather all it knows is that the ray being shot hits a certain number of objects. It must then go through all of them and find out which distance is the shortest.
#### 8 Anti-Aliasing
The chapter isn't too challenging to implement, but to summarize this entails shooting multiple rays to the same pixel, but offsets its direction based off of the center of the pixel. Since we're using the center of a pixel, we are allowed to change the ray's trajectory by .5 of a pixel in any direction. Simply add up the color values obtained from the multiple rays and divide it by the number of rays to average it.

#### 9.1 A Simple Diffuse Material
According to the text, the following method thats about to be described is meant to be an introduction to diffuse materials, and there are much better ways that will be learned later.

Diffusion in this case means that the ray has an equal chance of bouncing in any direction away from the surface. This means we can simply use a random function to find a plausible unit vector that bounces off the surface. The only check we need to make sure is that it is pointing towards the correct hemisphere. To do so, we can use the surface normal pointing outwards in order to check if the dot product returns positive or negative. In the case that its positive, it means that the ray is bouncing away from the surface, and we can use the random vector. However, if it returns negative, then we can simply invert the vector and use that instead.

To note about obtaining a random unit vector, the textbook uses the very straight forward rejection method, where we essentially randomly pick a vector, but it must be inside the volume of the sphere. The rejection part comes in when the method chosen is essentially encapsulating the sphere in a cube, ranging all possible vectors to be in the cube. The math very straight forward, with all three axes having values ranging from -1 to 1, and not restricting it to the dimensions of a sphere. If the vector randomly obtained sits inside the sphere, then we can use it, however if its outside the sphere, then we cannot use it. Even though we are inevitably going to normalize this vector, the point is that area of the sphere occupies alone is not part of the sphere, and would create bias, or a higher chance for the rays to select the corners of the cube. There's simply more "nonuniform" volume for the random value to pick, being unfair to the parts of the sphere.

#### 9.3 Fixing Shadow Acne
Not a complicated topic but very interesting. There really are a lot of floating point errors. In the case of calculating the point of intersection, there are chances in which the point can go under the surface because of floating point precision, which will cause the ray to bounce around inside the sphere unintentionally. To avoid this, simply have the starting interval be a really small number, which the give value was .001. Essentially, it'll ignore the surface it went under, if it did anyways.

#### 9.4 True Lambertian Reflection
This is the better version of the diffuse as it groups the random rays in to a more realistic distribution. To do so, it is best to visualize how this works. We still need to obtain a random unit vector, however instead of sending a ray in that direction immediately, we must first add it to the outwards surface normal. Only then, the point in which the add vectors points to, we send the new ray to be casted. Just from some observation, there appears to be less noise for the cost of adding a vec3.

#### 9.5 Using Gamma Correction for Accurate Color Intensity
Just something to take note of, but it appears that a lot of computer programs assume that an image is already "gamma corrected". As a note, images with data that have not been transformed yet are said to be in linear space, while images with transformed data are in gamma space. I'm sure more will be cleared up later. This is a simple value that multiples the color of the rays by a constant, being between 0 and 1. To go from linear space to gamma space, all you need to do is take the square root of the RGB values when writing the value, while its still in the 0 to 1, pre-conversion to 0 to 255.

#### 10 Metal
As a preface to this section, the design concept involves having "abstract material class that encapsulates unique behavior". We could have one single function that contains a ton of parameters and tune them all to create the desire material, however this is impractical. To lock away a majority of the behavioral decisions inside a black box that has a predetermined concept is more efficient and easier to read. As a start, we will create a virtual function that materials we want to create will inherit from.

#### 10.2 A Data Structure to Describe Ray-Object Intersections
I'm not sure as to why this gets clarified so late into the book, but the class hit_record is used to avoid a bunch of arguments, essentially what we explained before but at a more surface level that I think rounds out the general purpose of having this as a separate class.

Something thats mandatory we need to do when dealing with circular logic with two classes constantly referencing each other is that we need to initialize an empty class of the same name, just to tell the compiler that it will be defined later, just for now that it does exist and will be used. In understanding forward declarations, you need to understand purpose and limitations. Forward declarations helps in saving a lot of time compiling the same code repeatedly. Normally, if you needed to use a class created in a different header file, you would need to include the file. However, we were to simply state the class' existence, the compiler would simply acknowledge this and return back to it later once the real header file gets compiled. A limitation to forward declaration is that this only works with pointers, references, or function calls that use the object type. You cannot access the variables or functions, as the complier does not know they exist yet. In this scenario, you do need to include the header file properly to use the variables and functions.

The forward declaration here is used to obtain the material in which the hittable object is, not caring about what the material class has internally, but just that we need to know what type of material it is. We don't ask for a specific variable of material, nor do we use a function yet. All we do is initialize and set it to a material.

#### 10.3 Modeling Light Scatter and Reflectance
I just went down a rabbit hole involving the specifics of how the calculation of the color is meant to behave, and now I'll try to type in a digestible manner. This involves the scatter function, which will vary for every material. In this case, we'll be going over the matte material.

When I first saw the function and how it got the color, I got confused as to how the end color resulted in objects' color as we wanted. At a glance, it seems like it's simply multiplying colors back to back and essentially mixing all the colors indiscriminately. Attempting to imagine what colors mixing paint would result in, I know that mixing a bunch of colors together results in a brownish color usually without fail. Now, its pretty clear that it's not a good mindset to immediately refer to real life examples, and rather what should be understood is the math taking place in the function. The way that this is set up, we are essentially computing a variable amount of matrix multiplication, as such order matters. We know that colors should only ever range between 0 and 1, as such as we continue to multiply the values together, doing decimal multiplication with another decimal will result in a smaller decimal. This means that we aren't mixing the colors necessarily, but rather with the more bounces we account for, the closer we get to a value of 0. This means that this function is more of a shadow calculator. This means that the result of the function returning the color, more or less will return a darker and darker shadow. As matrix multiplication is not commutative, order does matter, which on the very last iteration of this recursive function, we multiply our initially desired color by our shadow, and that is the color the pixel will be.

Something I wanted to mention was the case of if our ray only bounced twice and then return the color of the sky of (1, 1, 1) on the third bounce. This would result in a mix the first two colors, which apparently does make sense. I'm not sure as to how someone could replicate a light source that only bounces twice, but the best example I could think of is having a room be lit up with a colored light, and take an object in to the room. Regardless of the color, unless its the same color, I believe that the object would seem like it was recolored completely.

Reviewing this again, I see that function ray_color() recursively returned attenuation * ray_color(...), which i think a good way to think about it is that the more vec3 we compute together, the darker the value, which multiplying by the attenuation would slowly influence the color to bias towards its original albedo established when we initialize the object.

#### 10.4 Mirrored Light Reflection
The important thing to remember here is that during a dot product, what you receive is a scalar not a vector. The equation is very straight forward as long you understand the math visually. From the book, we are given an example of a vector hitting a metal surface, and the equation to find the reflection is `v + 2b`, which looking at Figure 15, it makes sense. The derived equation though is `v - 2 * dot(v, n) * n`, where v is the vector to be reflected, and n is the surface normal. The `v` in both equations represent the same thing, however `b` will be substituted with `dot(v, n) * n`. The dot product here is used to determine the length projected from `v` on to `n`. This value is essentially the distance a point is from the tangent of the surface its reflecting from. `n` is used here to turn the scalar obtained from the dot product back into a vector that points in the same direction as the surface normal. To understand the rest of the equation, its best to view the actual image as its the best way to understand, but to try and explain, imagine the vector being reflected following through past the point of contact on the surface, this would represent the `v`. As we are supposedly beneath the surface, we can find the accurate reflection by adding the projected distance we found with the dot product in the direction of the surface normal, hence the `2b`. The reason as to why we subtract in the modified equation is because we are accounting for finding the dot product of two vectors that do not have their origins in the same coordinate. Because they aren't the resulting dot product will be negative, and as such we cancel that out by subtracting.

#### 10.6 Fuzzy Reflection
A variable called fuzz is added to a reflected ray, which deviates the ray in biased towards the unit vectors' direction. Makes the reflection not a perfect reflection. A value of 0 would make the random unit vector essentially not deviate from its perfect reflected path, while a value of 1 would send it far off course considering the reflected ray we're using is also a unit vector itself.

#### 11.2 Snell's Law
Some researching done, the idea of being able to split a vector to a parallel and perpendicular component is know has vector decomposition. Imagine a 2D vector being broken down into an x and y component (vectors [x 0] and [0 y]). In Snell's Law in finding a refracted ray, to find the outcome of the refracted ray, we need to find the perpendicular and parallel components. Adding these components together would result in our desired ray. There are some trig identities happening in the book in this section, however it really glosses over the math behind it, and tells us to just move on.

I do want to expand a bit on vector decomposition, and its frame of reference. When working with 2D vectors in 2 dimensions only, it is more visually apparent seeing "one" solution when asking for a vector that's perpendicular and parallel. When we move to 3 dimensions however, it becomes less obvious, seeing that if we were to asking for a vector that's perpendicular to our ray, we would receive an entire plane of vectors. I'm not sure if that would cause problems in finding the solution as I have a feeling they would all work as long as you are consistent with vectors you use, but I think there's a better way to view vector decomposition. I feel like the best use of this is reframing our perspective of the vector, transforming our view such that the vector we want can be mapped on to a 2D plane, eliminating a dimension to make it much easier to work with. As we are then in the 2D plane again, we are able to find the vectors parallel and perpendicular with more confidence.

Something fun you can do with vector decomposition is potentially reframe your space around the decomposed vectors, and working in local space. To properly move from local to world, just multiply the corresponding basis matrix to the local vector, and to go from world to local, just take the dot product of the world vector and basis matrix

#### 11.3 Total Internal Reflection
Looking at Snell's Law, the equation:
$$
\sin \theta_2 = \frac{n_1}{n_2} \sin \theta_1
$$

There is a an angle in which this law cannot be solved. Sine can only ever return a value between -1 and 1, which if the fraction `n1/n2` is a value larger than 1, which this can happen if the ray is going from a denser material to a less dense material, then the equation is asking for:
$$
\sin \theta_2 = \frac{1.5}{1} \sin \theta_1
$$

This is asking for sin theta2 to find an angle that results in sine return a value greater than 1. For example, if sin theta1 is 90 degrees, then the value returned is 1, and as such it is asking for sin theta2 to find an angle that results in sine returning 1.5 value.

From here, I think it's best to go line by line in the code to explain what its doing.

`double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;`
The refraction index, or the value that a material has that determines the angle of refraction, is determined when it is initialized at runtime, to which the ray determines if its going from the material to air, or air to material depending on if its hitting the front face or back face. Air has an refractive index of approximately 1, so we can use the reciprocal here to get the value of moving from glass to air.

`double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);`
`cos_theta` can be obtained from the dot product, and to assure no floating point errors can happen, we know that the highest value that can be returned is 1, and as such we cap it to 1 with `fmin`.

`double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);`
A trig identity used to find `sin_theta` given `cos_theta`.

`bool cannot_refract = ri * sin_theta > 1.0`
Checking the validity of `sin_theta` through what was stated for Snell's Law.

`if (cannot_refract) `
	`direction = reflect(unit_direction, rec.normal); `
`else `
	`direction = refract(unit_direction, rec.normal, ri);`
Accurate to physics in real life, if the angle is invalid according to Snell's Law, they ray is instead reflected, bouncing inside the sphere, otherwise allow to refract and exit the sphere.

To note, in the case Snell's Law, the scenario we are experimenting with involves a ray going from one material to another. n1, the numerator, is the material that the ray starts in. n2, the denominator, is the material that the ray enters.

Also, it appears that in this ray tracer, the reflective index is something that determined statically at compile time, and as such the math to determine the refractive index is done when you create the object.

`Refractive index in vacuum or air, or the ratio of the material's refractive index over the refractive index of the enclosing media`

#### 11.4 Schlick Approximation
There is math that calculates what I presume the angle that someone has to look at a piece of glass such that it begins to reflect rather than be transparent. The book doesn't cover much of it, rather it just states that it exists, and moves on. The implementation isn't complicated, and leave the reflection up to change depending on the angle of the ray and its refractive index.

#### 11.5 Modeling a Hollow Glass Sphere
Not much to say here, just wanted to make some connections. Given that we are making a hollow glass sphere, there are two chances for Total Internal Reflection to occur, the point where the ray is inside the glass, entering a body of air.

#### 12.1 Camera Viewing Geometry
`vfov` refers to the vertical angle that our camera will see. The book states that h is implied to be: $$
h = \tan \frac{\theta}{2}
$$
The important part before being able to see this intuitively is to understand what part of the angle we are solving for. Looking at an angle from the side, we will need to split it down the middle. To be able to use SohCahToa, we will need solid values to plug. The book states that we should use the magnitude distance of 1 down the line we used to split the angle in half, although I'm sure this value can change depending on the distance from your camera to the viewport. Done correctly, we have now created 2 right triangles, knowing the length of the base being the distance the camera is from the viewport, and the angle being composed of half our vfov that we chose since we split it down the middle. Our goal here is to find the height of the triangle, which we can find using tan. We need to modify the equation since we are looking for the opposite side length:
$$ \tan \theta = \frac{opposite}{adjacent} \rightarrow opposite = \frac{\tan \theta}{adjacent}
$$
If we plug in the values accordingly, opposite = h, and adjacent = 1, we get:
$$
h = \tan\frac{\theta}{2}
$$
To get the actual viewport height, we must double the value of h, since we only solved for half of the viewport. However, something to note, h is not the length of the half the viewport, but rather a ratio of length of h in proportion to the distance of the camera away from the viewport. This means that along with multiplying h by 2, we must also multiply it by the value of adjacent, or the focal length.

The example that the book gives us has a visual representation of how these ratios work together. `R` being given the ratio of 45 degrees, we place the spheres -1 distance away, similar to the focal distance that the book gave us. The radius of these spheres are the length of R, making the diameter equivalent to the whole vertical distance of the screen.

Another potential visual trick, if our aspect ratio was 1:1, then we horizontally, we'd see from the center of the left sphere to the center of the right sphere. As the book has an aspect ratio of 16:9, we can see a little past the center of the sphere as expected.

#### 12.2 Positioning and Orienting the Camera
This chapter follows similar to when LearnOpenGL.com had assisted in creating the LookAt() function in OpenGL from scratch. The difference however is what comes after, needing to calculate each pixels' position.

Initially, I did not understand as to why we needed to multiply the viewport width and height by a unit vector, as I thought that knowing the size of viewport was enough to determine how much to increment each pixel. Thinking about it now, its clear that we need to get the size of the viewport as a vector as determining each position with vectors that are point up and right relative to the camera would make it easier to determine where each ray is pointing.

#### 13.1 A Thin Lens Approximation
> 1. The focus plane is orthogonal to the camera view direction.
> 2. The focus distance is the distance between the camera center and the focus plane.
> 3. The viewport lies on the focus plane, centered on the camera view direction vector.
> 4. The grid of pixel locations lies inside the viewport (located in the 3D world).
> 5. Random image sample locations are chosen from the region around the current pixel location.
> 6. The camera fires rays from random points on the lens through the current image sample location.

If we were to simulate a camera, we do not need to simulate the inner workings of a camera, and instead skip straight to the lens part, and reverse engineer how light interacts with the lens. Above, step 5 and 6 are the unique steps that will be implemented in the next sub section, clearing up if there are any confusions.

#### 13.2 Generating Sample Rays
Before continuing with understanding how the lens first, a clarification needs to be about how the size of the lens is determined in this ray tracer. A plausible way that can be done is letting a parameter of the camera class be the radius of the lens. As though this fine, the book opts for something they claim to be easier is identifying the angle of a cone, with it being orthogonal to the center of the viewport, and the circular base of the cone is the lens size.

Initially, our ray tracer used the pin hole method, where all rays come from a single point, going through the viewport uniformly. In attempt to simulate a lens, instead of a pinhole, we use the area of disk in which the ray can randomly start on any point of the circle. These rays still do go in to their corresponding pixel in the viewport, but with the starting point being random and less concentrated, we should get a more blurry image.

Something that needs to be implemented is then getting a random point that's on our lens. To do so, we must once again get the basis vectors of our lens, having a function that randomly gets a point inside a unit circle similar to how we got a random point in a sphere for reflecting rays, and scale down/up our random point with the basis vectors. This point will be where the ray starts, going to its designated pixel.
