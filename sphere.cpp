// ==========================================================================
// $Id: sphere.cpp,v 1.2 2018/02/17 03:21:09 jlang Exp $
// Simple structure for data
// ==========================================================================
// (C)opyright:
//
//   Jochen Lang
//   SITE, University of Ottawa
//   800 King Edward Ave.
//   Ottawa, On., K1N 6N5
//   Canada.
//   http://www.site.uottawa.ca
//
// Creator: jlang (Jochen Lang)
// Email:   jlang@site.uottawa.ca
// ==========================================================================
// $Log: sphere.cpp,v $
// Revision 1.2  2018/02/17 03:21:09  jlang
// Simplified shape hierarchy for simplifying modifications
//
// Revision 1.1  2018/01/28 05:45:57  jlang
// Switch to core
//
//
// ==========================================================================
#include "sphere.h"


Sphere::Sphere() : RenderShape() {
  // 12 vertices
  d_vertex.insert(d_vertex.end(), {
      0.0f, 0.8506508f, 0.5257311f, 
	0.0f, 0.8506508f, -0.5257311f, 
	0.0f, -0.8506508f, 0.5257311f, 
	0.0f, -0.8506508f, -0.5257311f, 
	0.8506508f, 0.5257311f, 0.0f, 
	0.8506508f, -0.5257311f, 0.0f, 
	-0.8506508f, 0.5257311f, 0.0f, 
	-0.8506508f, -0.5257311f, 0.0f, 
	0.5257311f, 0.0f, 0.8506508f, 
	-0.5257311f, 0.0f, 0.8506508f, 
	0.5257311f, 0.0f, -0.8506508f, 
	-0.5257311f, 0.0f, -0.8506508f
	});
  // 20 faces
  d_index.insert(d_index.end(), {
    1, 0, 4, 
	0, 1, 6, 
	2, 3, 5, 
	3, 2, 7, 
	4, 5, 10, 
	5, 4, 8, 
	6, 7, 9, 
	7, 6, 11, 
	8, 9, 2, 
	9, 8, 0, 
	10, 11, 1, 
	11, 10, 3, 
	0, 8, 4, 
	0, 6, 9, 
	1, 4, 10, 
	1, 11, 6, 
	2, 5, 8, 
	2, 9, 7, 
	3, 10, 5, 
	3, 7, 11
    });
  // direct specification with all faces unrolled
  // - not supplied
}

/* given triangle (a,b,c)
 calculate three new midpoints
 x = 1/2(a+b)
 y = 1/2(b+c)
 z = 1/2(c+a)

	a ---- x ---- b
	 \           /
      \         /
	   z       y
	    \     /
		 \   /
		   c
  
 normalize each new point x' = x/||x|| so they sit on the unit sphere
 create 4 new triangles:
 T1 (a,x,z)
 T2 (b,y,x)
 T3 (c,z,y)
 T4 (x,y,z)
 replace old vertex, old index with new sets
 fancier way to do? --> ref
  http://www.multires.caltech.edu/pubs/sig99notes.pdf
  https://en.wikipedia.org/wiki/Subdivision_surface
  Maybe just apply a transform that doubles points, rotates each different direction?

*/

Sphere::Sphere(Sphere & _s) : RenderShape() {

	// copy over existing points
	for (auto it = _s.d_vertex.cbegin(); it != _s.d_vertex.cend(); ++it) {
		d_vertex.insert(d_vertex.cend(), *it);
	}

	int old_vertex_count = _s.getNPoints() / 3; // divide by 3 since each point has 3 coordinates and getNPoints() returns # of floats
	int vertex_count = old_vertex_count;

	// how many new points? n choose 2?
	// for each pair of points in _s, 1 new mid point
	// no. only add new midpoint between adjacent pairs
	// icosahedron is regular of degree 3
	// handhsake lemma
	// sum_1^N deg(G) = 2 |E|
	// sum_1^N 3 = 2 E
	// E = 3N/2
	// so 3N/2 new points created (one for each edge)
	// if the shape assumed 3-regular
	// after this process, though, each vertex doubles in degree?
	// calculation only works once. Need to know level of recursion
	int new_vertex_count = vertex_count + (3 * vertex_count) / 2;

	// keep track of indices for each midpoint in symmetric matrix
	// where M[i][j] = index of vertex between i and j = M[j][i]
	GLushort ** m = new GLushort*[vertex_count];
	for (int i = 0; i < vertex_count; ++i)
		m[i] = new GLushort[vertex_count](); // default initialized to zero

	// add new midpoints to the end of the vertex list
	// build new triangles using new indices
	for (int i = 0; i < _s.getNIndices() - 2; i += 3) {
		// get indices for a triangle
		GLushort p1 = _s.getIndex(i);
		GLushort p2 = _s.getIndex(i + 1);
		GLushort p3 = _s.getIndex(i + 2);

#ifdef JOEDEBUG2
		std::cerr << "p1: " << p1 << std::endl;
		std::cerr << "p2: " << p2 << std::endl;
		std::cerr << "p3: " << p3 << std::endl;
#endif
		// get vertices
		glm::vec3 v1 = _s.getVertex(p1);
		glm::vec3 v2 = _s.getVertex(p2);
		glm::vec3 v3 = _s.getVertex(p3);

#ifdef JOEDEBUG2
		std::cerr << "v1: " << glm::to_string(v1) << std::endl;
		std::cerr << "v2: " << glm::to_string(v2) << std::endl;
		std::cerr << "v3: " << glm::to_string(v3) << std::endl;

#endif
		if (m[p1][p2] == 0) {
			glm::vec3 mid = v1 + v2;
			mid *= 0.5;
			mid = glm::normalize(mid);
			d_vertex.insert(d_vertex.end(), { mid.x, mid.y, mid.z });
			m[p1][p2] = vertex_count;
			m[p2][p1] = vertex_count;
#ifdef JOEDEBUG2
			std::cerr << "new midp " << m[p1][p2] << " " << glm::to_string(mid) << std::endl;
#endif
			++vertex_count;
		}
		//glm::vec3 v1v2 = new_s.getVertex(m[p1][p2]);

		if (m[p1][p3] == 0) {		
			glm::vec3 mid = v1 + v3;
			mid *= 0.5;
			mid = glm::normalize(mid);
			d_vertex.insert(d_vertex.end(), { mid.x, mid.y, mid.z });
			m[p1][p3] = vertex_count;
			m[p3][p1] = vertex_count;
			++vertex_count;
#ifdef JOEDEBUG2
			std::cerr << "new midp " << m[p1][p3] << " " << glm::to_string(mid) << std::endl;
#endif

		}
		//glm::vec3 v1v3 = new_s.getVertex(m[p1][p3]);

		if (m[p2][p3] == 0) {
			glm::vec3 mid = v2 + v3;
			mid *= 0.5;
			mid = glm::normalize(mid);
			d_vertex.insert(d_vertex.end(), { mid.x, mid.y, mid.z });
			m[p2][p3] = vertex_count;
			m[p3][p2] = vertex_count;
			++vertex_count;
#ifdef JOEDEBUG2
			std::cerr << "new midp " << m[p2][p3] << " " << glm::to_string(mid) << std::endl;
#endif

		}
		//glm::vec3 v2v3 = new_s.getVertex(m[p2][p3]);

		// create new triangles

		d_index.insert(d_index.end(), {
			p1, m[p1][p2], m[p1][p3],
			p2, m[p2][p3], m[p1][p2],
			p3, m[p1][p3], m[p2][p3],
			m[p1][p2], m[p2][p3], m[p1][p3]
		});
	}

#ifdef JOEDEBUG2
	// debug
	std::cerr << " index to new midpoint matrix: " << std::endl;
	for (int i = 0; i < old_vertex_count; ++i) {
		for (int j = 0; j < old_vertex_count; ++j) {
			std::cerr << m[i][j] << " ";
		}
		std::cerr << std::endl;
	}
	std::cerr << "new vertices" << std::endl;
	int count = 0;
	for (auto it = d_vertex.cbegin(); it != d_vertex.cend(); ++it) {
		std::cerr << *it;
		if (it != d_vertex.cbegin() && (count-1) % 3 == 0)
			std::cerr << std::endl;
		else
			std::cerr << ", ";
		++count;
	}
	std::cerr << "end of list of vertices. count: " << count << std::endl;
	
	std::cerr << "new indices" << std::endl;
	count = 0;
	for (auto it = d_index.cbegin(); it != d_index.cend(); ++it) {
		std::cerr << *it;
		if (count > 0 && (count + 1) % 3 == 0) {
			std::cerr << std::endl;
		}
		else {
			std::cerr << ", ";
		}
		++count;
	}
	std::cerr << "end of list of vertices. count: " << count << std::endl;
	std::cerr << " expected num of new points: " << old_vertex_count + new_vertex_count << std::endl;
	std::cerr << " actual : " << vertex_count << std::endl;
	std::cerr << " expected num of new triangles: " << _s.getNIndices() / 3 * 4 << std::endl;
	std::cerr << " actual : " << static_cast<float>(d_index.size() / 3) << std::endl;
	std::cerr << " d_index.size() : " << d_index.size() << std::endl;
	std::cerr << " d_vertex.size() : " << d_vertex.size() << std::endl;
#endif

	// free memory used to track midpoint indices
	for (int i = 0; i < old_vertex_count; ++i) {
		delete[] m[i];
	}
	delete[] m;

}

// transform in place
void Sphere::subdivide() {

	// copy over existing points 
	//for (auto it = _s.d_vertex.cbegin(); it != _s.d_vertex.cend(); ++it) {
	//	d_vertex.insert(d_vertex.cend(), *it);
	//}

	int old_vertex_count = getNPoints() / 3; // divide by 3 since each point has 3 coordinates and getNPoints() returns # of floats
	int vertex_count = old_vertex_count;
	std::vector<GLushort> new_d_index;

	// how many new points? n choose 2?
	// for each pair of points in _s, 1 new mid point
	// no. only add new midpoint between adjacent pairs
	// icosahedron is regular of degree 3
	// handhsake lemma
	// sum_1^N deg(G) = 2 |E|
	// sum_1^N 3 = 2 E
	// E = 3N/2
	// so 3N/2 new points created (one for each edge)
	// if the shape assumed 3-regular
	// after this process, though, each vertex doubles in degree?
	// calculation only works once. Need to know level of recursion
	//int new_vertex_count = vertex_count + (3 * vertex_count) / 2;

	// keep track of indices for each midpoint in symmetric matrix
	// where M[i][j] = index of vertex between i and j = M[j][i]
	GLushort ** m = new GLushort*[vertex_count];
	for (int i = 0; i < vertex_count; ++i)
		m[i] = new GLushort[vertex_count](); // default initialized to zero

											 // add new midpoints to the end of the vertex list
											 // build new triangles using new indices
	for (int i = 0; i < getNIndices() - 2; i += 3) {
		// get indices for a triangle
		GLushort p1 = getIndex(i);
		GLushort p2 = getIndex(i + 1);
		GLushort p3 = getIndex(i + 2);

#ifdef JOEDEBUG2
		std::cerr << "p1: " << p1 << std::endl;
		std::cerr << "p2: " << p2 << std::endl;
		std::cerr << "p3: " << p3 << std::endl;
#endif
		// get vertices
		glm::vec3 v1 = getVertex(p1);
		glm::vec3 v2 = getVertex(p2);
		glm::vec3 v3 = getVertex(p3);

#ifdef JOEDEBUG2
		std::cerr << "v1: " << glm::to_string(v1) << std::endl;
		std::cerr << "v2: " << glm::to_string(v2) << std::endl;
		std::cerr << "v3: " << glm::to_string(v3) << std::endl;

#endif
		if (m[p1][p2] == 0) {
			glm::vec3 mid = v1 + v2;
			mid *= 0.5;
			mid = glm::normalize(mid);
			d_vertex.insert(d_vertex.end(), { mid.x, mid.y, mid.z });
			m[p1][p2] = vertex_count;
			m[p2][p1] = vertex_count;
#ifdef JOEDEBUG2
			std::cerr << "new midp " << m[p1][p2] << " " << glm::to_string(mid) << std::endl;
#endif
			++vertex_count;
		}
		//glm::vec3 v1v2 = new_s.getVertex(m[p1][p2]);

		if (m[p1][p3] == 0) {
			glm::vec3 mid = v1 + v3;
			mid *= 0.5;
			mid = glm::normalize(mid);
			d_vertex.insert(d_vertex.end(), { mid.x, mid.y, mid.z });
			m[p1][p3] = vertex_count;
			m[p3][p1] = vertex_count;
			++vertex_count;
#ifdef JOEDEBUG2
			std::cerr << "new midp " << m[p1][p3] << " " << glm::to_string(mid) << std::endl;
#endif

		}
		//glm::vec3 v1v3 = new_s.getVertex(m[p1][p3]);

		if (m[p2][p3] == 0) {
			glm::vec3 mid = v2 + v3;
			mid *= 0.5;
			mid = glm::normalize(mid);
			d_vertex.insert(d_vertex.end(), { mid.x, mid.y, mid.z });
			m[p2][p3] = vertex_count;
			m[p3][p2] = vertex_count;
			++vertex_count;
#ifdef JOEDEBUG2
			std::cerr << "new midp " << m[p2][p3] << " " << glm::to_string(mid) << std::endl;
#endif

		}
		//glm::vec3 v2v3 = new_s.getVertex(m[p2][p3]);

		// create new triangles

		new_d_index.insert(new_d_index.end(), {
			p1, m[p1][p2], m[p1][p3],
			p2, m[p2][p3], m[p1][p2],
			p3, m[p1][p3], m[p2][p3],
			m[p1][p2], m[p2][p3], m[p1][p3]
		});
	}

#ifdef JOEDEBUG2
	// debug
	std::cerr << " index to new midpoint matrix: " << std::endl;
	for (int i = 0; i < old_vertex_count; ++i) {
		for (int j = 0; j < old_vertex_count; ++j) {
			std::cerr << m[i][j] << " ";
		}
		std::cerr << std::endl;
	}
	std::cerr << "new vertices" << std::endl;
	int count = 0;
	for (auto it = d_vertex.cbegin(); it != d_vertex.cend(); ++it) {
		std::cerr << *it;
		if (it != d_vertex.cbegin() && (count - 1) % 3 == 0)
			std::cerr << std::endl;
		else
			std::cerr << ", ";
		++count;
	}
	std::cerr << "end of list of vertices. count: " << count << std::endl;

	std::cerr << "new indices" << std::endl;
	count = 0;
	for (auto it = d_index.cbegin(); it != d_index.cend(); ++it) {
		std::cerr << *it;
		if (count > 0 && (count + 1) % 3 == 0) {
			std::cerr << std::endl;
		}
		else {
			std::cerr << ", ";
		}
		++count;
	}

	std::cerr << "end of list of vertices. count: " << count << std::endl;
	std::cerr << " expected num of new points: " << old_vertex_count + new_vertex_count << std::endl;
	std::cerr << " actual : " << vertex_count << std::endl;
	std::cerr << " expected num of new triangles: " << _s.getNIndices() / 3 * 4 << std::endl;
	std::cerr << " actual : " << static_cast<float>(d_index.size() / 3) << std::endl;
	std::cerr << " d_index.size() : " << d_index.size() << std::endl;
	std::cerr << " d_vertex.size() : " << d_vertex.size() << std::endl;
#endif

	// swap old d_list with new
	d_index.swap(new_d_index);

	// free memory used to track midpoint indices
	for (int i = 0; i < old_vertex_count; ++i) {
		delete[] m[i];
	}
	delete[] m;

}
/*
Sphere& Sphere::subdivide(Sphere &_s, int n) {
	assert(n >= 0);
	if (n == 0) {
		return _s;
	}
	else {
		return subdivide(Sphere(_s), n - 1);
	}
}
*/
