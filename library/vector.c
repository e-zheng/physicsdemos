#include "vector.h"
#include <math.h>

const vector_t VEC_ZERO = {0.0, 0.0};

vector_t vec_add(vector_t v1, vector_t v2){
	vector_t v = {v1.x + v2.x, v1.y + v2.y};
	return v;
}

vector_t vec_subtract(vector_t v1, vector_t v2){
	vector_t v = {v1.x - v2.x, v1.y - v2.y};
	return v;
}

vector_t vec_multiply(double scalar, vector_t v){
	vector_t v_mult = {scalar * v.x, scalar * v.y};
	return v_mult;
}

vector_t vec_negate(vector_t v){
	vector_t v_neg = vec_multiply(-1, v);
	return v_neg;
}

double vec_dot(vector_t v1, vector_t v2){
	return v1.x * v2.x + v1.y * v2.y;
}

double vec_cross(vector_t v1, vector_t v2){
	return v1.x * v2.y - v1.y * v2.x;
}

vector_t vec_rotate(vector_t v, double angle){
	vector_t v_rot ={v.x * cos(angle) - v.y * sin(angle), v.x * sin(angle) + v.y * cos(angle)};
	return v_rot;
}
