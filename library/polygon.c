#include "polygon.h"
#include "vector.h"
#include "list.h"
#include <stdio.h>

const double POLYGON_CONSTANT = 6;

double polygon_area(list_t *polygon){
	double area = 0;
	for(size_t i = 0; i < list_size(polygon); i++){
		area += vec_cross(*((vector_t*) list_get(polygon, i)), *((vector_t*) list_get(polygon, (i + 1) % list_size(polygon))));
	}
	return area/2;
}

vector_t polygon_centroid(list_t *polygon){
	vector_t centroid = VEC_ZERO;
	for(size_t i = 0; i < list_size(polygon); i++){
		vector_t *v1 = (vector_t*) list_get(polygon, i);
		vector_t *v2 = (vector_t*) list_get(polygon, (i + 1) % list_size(polygon));
		centroid.x += vec_multiply(vec_cross(*v1, *v2), vec_add(*v1, *v2)).x;
		centroid.y += vec_multiply(vec_cross(*v1, *v2), vec_add(*v1, *v2)).y;
	}
	return vec_multiply(1/(POLYGON_CONSTANT * polygon_area(polygon)), centroid);
}

void polygon_translate(list_t *polygon, vector_t translation){
	for(size_t i = 0; i < list_size(polygon); i++){
		vector_t add = vec_add(*((vector_t*) list_get(polygon, i)), translation);
		((vector_t*) list_get(polygon, i))->x = add.x;
		((vector_t*) list_get(polygon, i))->y = add.y;
	}
}

void polygon_rotate(list_t *polygon, double angle, vector_t point){
	for(size_t i = 0; i < list_size(polygon); i++){
		vector_t rot = vec_add(vec_rotate(vec_subtract(*((vector_t*) list_get(polygon, i)), point), angle), point);
		((vector_t*) list_get(polygon, i))->x = rot.x;
		((vector_t*) list_get(polygon, i))->y = rot.y;
	}
}
