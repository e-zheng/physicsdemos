#include "list.h"
#include "collision.h"
#include "polygon.h"
#include "vector.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

const double Y_BOUND = 500;
const double PROJECTION_LIST_SIZE = 2;
const double DOUBLE_MIN = -1000;
const double DOUBLE_MAX = 1000;
const double CLOSE_TO_ZERO = 0.0001;

double min_overlap = DOUBLE_MAX;
double axis = DOUBLE_MAX;

vector_t project_points(list_t *shape, double slope){
	double min = DOUBLE_MAX;
	double max = DOUBLE_MIN;
	double magnitude = sqrt(1 + pow(slope, PROJECTION_LIST_SIZE));
	double unit_x = 1/magnitude;
	double unit_y = slope/magnitude;
	for (size_t p = 0; p < list_size(shape); p++){
		vector_t *temp = list_get(shape, p);
		double dot = (unit_x * temp->x) + (unit_y * temp->y);
		if (dot < min) {min = dot;}
		if (dot > max) {max = dot;}
	}
    vector_t solution = (vector_t) {min, max};
	return solution;
}

bool iterate_points(list_t *shape_ref, list_t *other_shape){
	vector_t *temp_point1 = list_get(shape_ref, list_size(shape_ref) - 1);
	for (size_t i = 0; i < list_size(shape_ref); i++){
		vector_t *temp_point2 = list_get(shape_ref, i);
		double temp_slope = (temp_point1->y - temp_point2->y)/(temp_point1->x - temp_point2->x);
        double new_slope = DOUBLE_MAX;
		if (fabs(temp_slope) > CLOSE_TO_ZERO){
		    new_slope = -1/temp_slope;
		}
		vector_t polygon_dots1 = project_points(shape_ref, new_slope);
		vector_t polygon_dots2 = project_points(other_shape, new_slope);
		// if there is no intersection, polygons do not collide
		double x = 0;
		double y = 0;
		if (polygon_dots1.x > polygon_dots2.x){
			x = polygon_dots1.x;
		}
		else {
			x = polygon_dots2.x;
		}
		if (polygon_dots1.y < polygon_dots2.y){
			y = polygon_dots1.y;
		}
		else {
			y = polygon_dots2.y;
		}
		if (y - x < min_overlap){
			min_overlap = y-x;
			axis = new_slope;
		}
		if ((polygon_dots1.y < polygon_dots2.x || polygon_dots2.y < polygon_dots1.x)){
			return false;
		}
		temp_point1 = list_get(shape_ref, i);
	}
	return true;
}

collision_info_t *find_collision(list_t *shape1, list_t *shape2){
	min_overlap = DOUBLE_MAX;
	axis = 0;
    bool collision = iterate_points(shape1, shape2) && iterate_points(shape2, shape1);
    collision_info_t *result = malloc(sizeof(collision_info_t));
    result->collided = collision;
	double magnitude = sqrt(1 + pow(axis, PROJECTION_LIST_SIZE));
	if (fabs(magnitude) < CLOSE_TO_ZERO){ // in case magnitute is 0, do not want to divide by 0
		magnitude = CLOSE_TO_ZERO;
	}
	double unit_x = 1/magnitude;
	double unit_y = axis/magnitude;
    result->axis = (vector_t){unit_x, unit_y};
	return result;
}
