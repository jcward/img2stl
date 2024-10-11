# img2stl - depthmap image (.tga or .png) to STL 3D mesh generator
This is a stand-alone command-line utility (forked from [TGA2STL](https://github.com/DEF7/TGA2STL)) for generating STL 'stereolithography' meshes from depthmap images. This is useful for anybody who wishes to use their existing CAM or 3D-printer model-slicing software for milling/printing out a 3D surface that is described using a grayscale depth/height map, and require a 3D mesh as input for their software. img2stl utilizes an algorithm derived from one that was used during the pre-GPU days of real-time terrain LOD rendering called ROAM (Real-time Optimally Adaptive Meshing). The purpose of this program is to allow for light-weight meshes with a fewer number of triangles to depict the surface described by an input depthmap, wasting as few vertices and triangles as possible. This is accomplished by effectively only adding triangles where the surface varies, and neglecting areas that are more planar.

![spiralpyramid.tga, thresh = 0.0625, 36k triangles](image_example.jpg)

img2stl is very fast. It can generate 100k triangle meshes in a fraction of a second (less than 500ms) on a 3.5ghz machine. Many of the existing grid-based depthmap-to-mesh programs out there are extremely slow because of the language they're written in and/or because they generate huge unmanageable and unwieldy meshes with tons of triangles.

There are several depthmap/heightmap-to-mesh programs out there but the meshes that they output are cumbersome to work with. These programs strictly generate a grid of triangles with vertex heights dictated by input image pixel brightness. In contrast, img2stl starts with a coarse grid of triangles and subdivides them when they do not conform to the heightmap to within a threshold value. The end result is a mesh that is more friendly to 3D programs, such as CAM software, 3D printing model-slicers, and modeling/animation packages. By only creating smaller triangles where there is more detail and 'variance' the mesh uses only as many triangles as it needs to osculate the surface represented by the 2D depthmap.


# Example Output

![test depthmap](image_depthmap.jpg)

Here's a 512x512 depthmap I've been using for testing. It is the input used for the following output mesh images. These are closeups for purposes of conveying the changes in mesh triangulation as the threshold value decreases.

![thresh 0.25](image_thresh0.25.jpg)

This is the mesh generated from using a subdivide threshold of 0.25 with all other parameters at their defaults. (39k triangles)

![thresh 0.125](image_thresh0.125.jpg)

Here's the mesh output with thresh set to 0.125. (78k triangles)

![thresh 0.125](image_thresh0.0625.jpg)

...and 0.0625. (135k triangles)

You can see that by halving the subdivision threshold value the output mesh triangle count doubles.


# What is a TGA?
TGA (or 'Targa') is a lossless image format that was more popular in the past. It's a simple format to code around for reading or writing images. However, TGA files can be large due to their raw pixel data format. A 24-bit 256x256 TGA is 193KB.

Virtually every raster imaging program supports loading/saving Targa TGA images, including GIMP and Photoshop. If you have depthmaps that are in another format you should have no trouble opening them in either of these programs and exporting to a non-RLE TGA.


# Usage

Linux usage of img2stl is displayed when running the program with no arguments:

```
usage: img2stl <imgpath> [-thresh #.#] [-smooths #] [-hscale #] [-vscale #]

 thresh: maximum mesh/heightmap error (default = 0.10)
 smooths: smoothing passes on heightmap data before meshing. (default = 2)
 hscale: horizontal scale, pixels-per-inch. (default = 128)
 vscale: vertical scaling, image gradiations-per-inch. (default = 256)
```

'thresh' designates the threshold at which the mesh's deviation from the depthmap image input triggers a triangle subdivide.

'smooths' sets the number of simple 3x3 depth value averages to perform across the input image data before generating the mesh. This helps to prevent extraneous subdivisions from occurring, minimizing triangle-count. However, this is at the expense of retaining detail in the final mesh.

'hscale' allows establishing how many pixels to each mesh unit. If you have a 512x512 image, for instance, and set hscale to 128 then the output mesh will be 4x4 model units. I personally interpret them as inches, and so hscale can be considered pixels-per-inch.

'vscale', similarly, establishes how many mesh units the RGB 0-255 range should span. The default is 256, thus the full range of an image's 0-255 color range is capable of generating a mesh with a vertical size of one mesh unit. If you want a shallower mesh then use a larger number. For a mesh that's 0.25 units tall you would use 1024, as the 0-255 range goes into 1024 four times, and is thus 0.25 x 1024.

If img2stl complains about an invalid TGA format it is likely due to the TGA being run-length encoded. You must find a way to convert it to the raw pixel data formatted TGA, either by loading it in an imaging program and re-exporting it or using a converter of some kind (if one exists?).


# Limitations and Requirements
There are some limitations as to what img2stl can work with insofar as the depthmap images themselves are concerned:

- Run-length encoded (RLE) Depthmap TGA files cannot be used. Only raw-format TGA images will be recognized by img2stl in its current form. The origin can, however, be top-left or bottom-left (some imaging programs like to default to either, which can make loading TGA's tricky).

- Depthmap dimensions must be a power of two. It *will* work with non-power-of-two dimensions, but you'll get a gross pixellated stair-stepping resultant mesh, which is a product of the nearest sampling of the depthmap data. With the addition of a linear interpolation, at the very least, this restriction would be resolved.

- Rectangular depthmaps (i.e. non-square) *do* work but the mesh division doesn't adapt to the image's aspect ratio to keep subdivisions square. The starting base triangle mesh is initialized to the input image aspect ratio, but divisions still occur at halfway points vertically and horizontally, and are not distributed in a absolute homogenous coordinate space. I'm not sure if this is something I'm interested in ever resolving. A temporary fix would be to take your rectangular depthmaps and add margins on the longer sides to make it the image square.


# Possible Feature Additions and Enhancements
This program is pretty much just as I originally wrote it. I tweaked a few parameters and the algorithm for sub-dividing triangles, but other than that this is virtually it's one and only incarnation. There are many things that can be done to improve it as a user application. Here are some of the ideas I've had, and they are in no particular order:

- Support larger images and bigger meshes with more triangles. This version of img2stl only allows meshes up to 64MB before it whines about an overflow. It also will only allow for meshes with up to 16 million triangles. I haven't calculated which comes first, 64MB or 16m triangles, but whatever is hit first will cause it to stop subdividing and/or outputting anymore triangles beyond whatever it achieved up to that point. The problem lies in that it statically allocates fixed-sized buffers to work inside of. This can be resolved by streamlining the whole program so that after it generates the binary tree of triangle subdivisions it directly writes each triangle to the STL mesh file, instead of dumping triangles to a buffer and then dumping those triangles to an STL file buffer, and then dumping that to disk.

- 3D preview with real-time parameter adjustment. This could almost take the form of just an external program that calls img2stl and re-loads the mesh each time a new one is spit out.

- Argument to designate the 'origin' of the mesh. Currently the origin lies at the bottom left corner of the mesh, which is really only standard for raster images and displays, but not CAM. I intend to add arguments for designating the XY position of the origin on the mesh as a 0.0-1.0 fraction of the mesh size. So for the origin to be in the center you would use an XY origin of *0.5,0.5* and that would be simple enough.

- Bilinear interpolation to allow for non-power-of-two images without the yucky terracing effect that nearest-neighbor causes during sampling at subdivision intervals.

- Create more intuitive scaling argument conventions. The vscale/hscale makes sense to me, but it might not be so clear to the next person. Perhaps the vscale argument should be inverted, so that it represents the number of brightness gradiations that fit into one mesh unit. In other words, instead of '-vscale 512' to make the mesh only 0.5 units tall, use '-vscale 0.5'. As for hscale, perhaps it could just be the number of mesh units for the width of the image, instead of mucking about with correlating the two as a ratio. Then if you wanted your 32x32 depthmap to be 10 mesh units wide, you could, easily.

