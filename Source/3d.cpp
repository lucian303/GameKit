/*#include <math.h>
#include "..\h\fixed.h"

#define MAX_VERTICES_PER_POLY 3
#define MAX_POINTS_PER_OBJECT 512

#define NO_SHADE 1
#define FLAT_SHADE 2
#define GOURAUD_SHADE 3
#define TEXTURE_SHADE 4
#define GOURAUDTEXTURE_SHADE 5

//calculate sin + cosine tables
//init 3d
//etc
//Basic definitions may be chagnged

class Point3d {
	//protected:
	public: //for now
	fixed x, y, z, u, v;
};

class Angle3d {
	public:
	long x, y, z;
}

class Polygon3d {
	public:
	int Vertices [MAX_VERTICES_PER_POLY];
	int color;
	int visible;
	int active;
	int twosides;
	int shading;
	TextureMap TheMap;
};

class Object3d {
	//protected:
	public:
	Point3d Scratch[MAX_POINTS_PER_OBJECT];
	Point3d TheVertices[MAX_POINTS_PER_OBJECT];
	char * name;
	int numvertices;
	int numpolys;
	int state;
	Polygon3d polys [MAX_POLYS_PER_OBJECT];
	Point3d position;
	Angle3d rotation;
};

class TextureMap {
	public:
	char * map;

};


/*class ProcessedPoly3d {

	Point3d points[MAX_VERTICES_PER_POLY];
	int color;
	int visible;
	int active;
	int twosides;
	int shading;

};
*/
