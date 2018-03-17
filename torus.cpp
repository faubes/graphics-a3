// ==========================================================================
// $Id: torus.cpp,v 1.2 2018/02/17 03:21:13 jlang Exp $
// Simple structure for torus data
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
// $Log: torus.cpp,v $
// Revision 1.2  2018/02/17 03:21:13  jlang
// Simplified shape hierarchy for simplifying modifications
//
// Revision 1.1  2018/01/28 05:45:57  jlang
// Switch to core
//
//
// ==========================================================================
#include "torus.h"

Torus::Torus() : RenderShape() {
        d_vertex.insert(d_vertex.end(), {
                                0.789861f, -0.10218f, -0.911124f,
                                0.561088f, -0.0842026f, -0.492652f,
                                0.881559f, 0.110028f, -0.900394f,
                                0.636476f, -0.184782f, -0.774557f,
                                0.971799f, 0.157076f, -0.700116f,
                                1.12888f, -0.322217f, -0.467433f,
                                0.93792f, -0.387658f, -0.572775f,
                                0.990121f, -0.0769495f, -0.1286f,
                                1.16745f, -0.169317f, -0.258374f,
                                0.826698f, -0.00451939f, -0.25513f,
                                1.02473f, -0.663975f, -0.024622f,
                                0.666283f, -0.299505f, -0.254595f,
                                1.15876f, -0.453875f, 0.0943763f,
                                0.889194f, -0.584793f, -0.233071f,
                                0.662259f, -0.535993f, 0.00667413f,
                                0.668888f, -0.312894f, 0.207518f,
                                0.545168f, -0.806521f, 0.497487f,
                                0.916274f, -0.320989f, 0.423008f,
                                1.00199f, -0.522515f, 0.465809f,
                                0.905451f, -0.750479f, 0.463125f,
                                0.353232f, -0.689554f, 0.51357f,
                                0.624655f, -0.388184f, 0.795286f,
                                0.701708f, -0.612159f, 0.832565f,
                                0.347114f, -0.588569f, 1.05254f,
                                0.283669f, -0.332604f, 0.662163f,
                                0.342463f, -0.792375f, 0.878458f,
                                0.150583f, -0.763265f, 0.757626f,
                                -0.0268654f, -0.249733f, 1.08738f,
                                -0.0620225f, -0.444743f, 1.16475f,
                                0.16238f, -0.502159f, 0.597316f,
                                -0.287528f, -0.559062f, 1.08385f,
                                -0.383859f, -0.560388f, 0.811338f,
                                -0.667335f, 0.0672933f, 0.954728f,
                                -0.545705f, -0.201367f, 1.10513f,
                                -0.433311f, -0.277979f, 0.618683f,
                                -0.245078f, -0.0911976f, 0.785996f,
                                -0.79087f, -0.317908f, 0.866665f,
                                -0.716385f, 0.0875567f, 0.262052f,
                                -0.551481f, 0.149066f, 0.771945f,
                                -1.10184f, 0.0813328f, 0.595184f,
                                -0.948687f, 0.133613f, 0.82038f,
                                -0.844392f, 0.393252f, 0.562831f,
                                -0.99656f, 0.00741998f, 0.250248f,
                                -0.656092f, 0.261012f, 0.320712f,
                                -0.817611f, 0.606736f, 0.12773f,
                                -0.956202f, 0.6643f, 0.0572629f,
                                -1.12726f, 0.473105f, 0.280983f,
                                -1.19573f, 0.290336f, 0.14837f,
                                -0.665253f, 0.56166f, -0.039276f,
                                -1.0855f, 0.614443f, -0.121271f,
                                -0.730271f, 0.754039f, -0.216901f,
                                -0.994967f, 0.590235f, -0.490928f,
                                -1.04865f, 0.270907f, -0.188865f,
                                -0.803591f, 0.793241f, -0.487069f,
                                -0.82545f, 0.253547f, -0.26528f,
                                -0.630749f, 0.375465f, -0.179134f,
                                -0.658187f, 0.568794f, -0.86608f,
                                -0.58234f, 0.40765f, -0.860925f,
                                -0.457031f, 0.316018f, -0.711336f,
                                -0.229264f, 0.338021f, -0.672112f,
                                -0.481612f, 0.738552f, -0.902176f,
                                -0.30788f, 0.786349f, -0.651295f,
                                0.0390651f, 0.574016f, -1.131f,
                                -0.2623f, 0.562111f, -0.529558f,
                                -0.305771f, 0.794066f, -0.847422f,
                                0.0702753f, 0.16308f, -0.876016f,
                                -0.186667f, 0.43437f, -1.11075f,
                                0.192568f, 0.428652f, -0.660646f,
                                0.0149692f, 0.723736f, -0.833153f,
                                0.439853f, 0.22321f, -1.13172f,
                                0.361218f, 0.0673639f, -1.02961f,
                                0.299397f, 0.11687f, -0.70136f,
                                0.563674f, 0.20064f, -0.5496f,
                                0.67572f, 0.344885f, -0.672245f,
                                0.32221f, 0.600793f, -0.9927f,
                                0.521436f, 0.379591f, -1.07452f
                        });
        d_index.insert(d_index.end(),{
                               4, 2, 75,
                               4, 75, 74,
                               73, 4, 74,
                               0, 70, 69,
                               2, 69, 75,
                               73, 74, 68,
                               72, 73, 67,
                               71, 72, 67,
                               3, 71, 65,
                               75, 62, 74,
                               70, 3, 65,
                               75, 69, 62,
                               73, 68, 61,
                               67, 73, 61,
                               71, 67, 59,
                               65, 71, 59,
                               70, 65, 57,
                               70, 57, 66,
                               69, 70, 66,
                               69, 66, 62,
                               74, 62, 64,
                               68, 74, 64,
                               67, 61, 63,
                               59, 67, 63,
                               65, 59, 58,
                               57, 65, 58,
                               64, 62, 60,
                               62, 66, 60,
                               68, 64, 61,
                               66, 57, 56,
                               60, 66, 56,
                               63, 61, 50,
                               64, 60, 53,
                               61, 64, 53,
                               53, 60, 51,
                               61, 53, 50,
                               59, 63, 55,
                               58, 59, 55,
                               57, 58, 54,
                               60, 56, 51,
                               63, 50, 48,
                               55, 63, 48,
                               58, 55, 54,
                               56, 57, 51,
                               50, 44, 48,
                               57, 54, 52,
                               51, 57, 52,
                               53, 51, 49,
                               53, 49, 45,
                               53, 45, 50,
                               44, 50, 45,
                               51, 52, 47,
                               49, 51, 47,
                               45, 49, 46,
                               49, 47, 46,
                               55, 48, 43,
                               55, 43, 37,
                               54, 55, 37,
                               54, 37, 42,
                               52, 54, 42,
                               45, 46, 41,
                               44, 45, 41,
                               48, 44, 43,
                               47, 52, 42,
                               46, 39, 40,
                               41, 46, 40,
                               46, 47, 39,
                               44, 41, 38,
                               43, 44, 38,
                               47, 42, 39,
                               41, 40, 32,
                               41, 32, 38,
                               43, 38, 37,
                               39, 42, 36,
                               37, 35, 34,
                               42, 37, 34,
                               39, 36, 40,
                               37, 38, 35,
                               42, 34, 31,
                               36, 42, 31,
                               40, 33, 32,
                               40, 36, 33,
                               32, 33, 27,
                               38, 32, 27,
                               35, 38, 27,
                               33, 36, 30,
                               33, 30, 28,
                               27, 33, 28,
                               36, 31, 30,
                               31, 34, 29,
                               35, 27, 21,
                               35, 21, 24,
                               34, 35, 24,
                               29, 34, 24,
                               27, 28, 23,
                               31, 29, 20,
                               26, 31, 20,
                               30, 31, 26,
                               30, 26, 25,
                               28, 30, 25,
                               28, 25, 23,
                               27, 23, 21,
                               23, 22, 21,
                               24, 21, 17,
                               26, 20, 16,
                               25, 26, 16,
                               23, 25, 22,
                               22, 25, 19,
                               24, 17, 15,
                               29, 24, 15,
                               29, 15, 20,
                               25, 16, 19,
                               22, 19, 18,
                               21, 22, 18,
                               21, 18, 17,
                               20, 15, 14,
                               16, 20, 14,
                               19, 16, 10,
                               18, 19, 12,
                               18, 12, 17,
                               16, 14, 13,
                               10, 16, 13,
                               19, 10, 12,
                               17, 12, 8,
                               17, 8, 7,
                               17, 7, 15,
                               14, 15, 11,
                               12, 5, 8,
                               15, 7, 9,
                               11, 15, 9,
                               12, 10, 5,
                               14, 11, 3,
                               13, 14, 3,
                               10, 13, 6,
                               10, 6, 5,
                               11, 9, 1,
                               8, 5, 4,
                               7, 8, 4,
                               7, 4, 73,
                               9, 7, 73,
                               3, 11, 1,
                               6, 13, 3,
                               4, 5, 2,
                               1, 9, 72,
                               5, 6, 0,
                               5, 0, 2,
                               72, 9, 73,
                               6, 3, 0,
                               1, 72, 71,
                               1, 71, 3,
                               0, 3, 70,
                               69, 2, 0
                       });
        // direct specification with all faces unrolled
        // - not supplied
}

// circles tracing out a circle
// r_o radius of donut hole
// r_i radius of donut size
// n_o frequency of circle
// n_i frequency of circles on ring
void Torus::reshape(GLfloat r_o, GLfloat r_i, GLushort n_o, GLushort n_i) {
        d_vertex.clear();
        d_index.clear();
		d_normal.clear();
        for (int i = 0; i < n_o; i++) {
                for (int j = 0; j < n_i; j++) {
                        GLfloat alpha = 2*i*glm::pi<float>() / n_i;
                        GLfloat beta = 2*j*glm::pi<float>() / n_o;
                        GLfloat x = (r_o + r_i*glm::cos(alpha))*glm::cos(beta);
                        GLfloat y = (r_o + r_i*glm::cos(alpha))*glm::sin(beta);
                        GLfloat z = (r_i*glm::sin(alpha));
                        d_vertex.insert(d_vertex.cend(), { x, y, z });

						// calculate normal vector
						// using formulas from 
						// http://web.cs.ucdavis.edu/~amenta/s12/findnorm.pdf
						
						/* tangent vector with respect to big circle */
						glm::vec3 t = glm::vec3(-glm::sin(beta), glm::cos(beta), 0.0f);
						/* tangent vector with respect to little circle */
						glm::vec3 s = glm::vec3(cos(beta)*(-sin(alpha)), sin(beta)*(-sin(alpha)), cos(alpha));
						/* normal is cross-product of tangents */
						glm::vec3 n = glm::normalize(glm::cross(s, t));
						/* normalize normal */						
						d_normal.insert(d_normal.cend(), {n.x, n.y, n.z});

						// calculate neighbours for new points to add triangle (quad) indices

                        GLushort a = i*n_i + j;
                        GLushort b = i*n_i + ((j+1) % n_o);
                        GLushort c = ((i+1) % n_o)*n_i + j;
                        d_index.insert(d_index.cend(), { a, b, c });
                        GLushort d = ((i+1) % n_o)*n_i + ((j + 1) % n_i);
                        d_index.insert(d_index.cend(), { c, b, d });

                }
        }
#ifdef JOEDEBUG
        for (int i = 0; i < getNPoints() / 3; ++i) {
                std::cerr << glm::to_string(getVertex(i)) << std::endl;
        }
#endif
}
