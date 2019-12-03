/* simple filters */

#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <cmath>
#include "image.h"

#define WIDTH 4.0

#pragma warning(disable: 4996)
#pragma warning(disable: 4995)
#pragma warning(disable: 4805)
#pragma warning(disable: 4267)

// convolve src with mask.  dst is flipped! 
static void convolve_even(image<float> *src, image<float> *dst, std::vector<float> &mask) {
	int width = src->width();
	int height = src->height();
	int len = mask.size();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float sum = mask[0] * imRef(src, x, y);
			for (int i = 1; i < len; i++) {
				sum += mask[i] *
					(imRef(src, std::max(x - i, 0), y) +
					imRef(src, std::min(x + i, width - 1), y));
			}
			imRef(dst, y, x) = sum;
		}
	}
}

// convolve src with mask.  dst is flipped! 
static void convolve_odd(image<float> *src, image<float> *dst, std::vector<float> &mask) {
	int width = src->width();
	int height = src->height();
	int len = mask.size();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float sum = mask[0] * imRef(src, x, y);
			for (int i = 1; i < len; i++) {
				sum += mask[i] *
					(imRef(src, std::max(x - i, 0), y) -
					imRef(src, std::min(x + i, width - 1), y));
			}
			imRef(dst, y, x) = sum;
		}
	}
}

// normalize mask so it integrates to one 
static void normalize(std::vector<float> &mask) {
	int len = mask.size();
	float sum = 0;
	for (int i = 1; i < len; i++) {
		sum += fabs(mask[i]);
	}
	sum = 2 * sum + fabs(mask[0]);
	for (int i = 0; i < len; i++) {
		mask[i] /= sum;
	}
}

// make filters 
#define MAKE_FILTER(name, fun)  static std::vector<float> make_ ## name (float sigma) { sigma = std::max(sigma, 0.01F);	int len = (int)ceil(sigma * WIDTH) + 1;  std::vector<float> mask(len); for (int i = 0; i < len; i++) {mask[i] = fun;} return mask;}
MAKE_FILTER(fgauss, exp(-0.5f*square(i / sigma)));


// convolve image with gaussian filter 
static image<float> *smooth(image<float> *src, float sigma) {
	std::vector<float> mask = make_fgauss(sigma);
	normalize(mask);

	image<float> *tmp = new image<float>(src->height(), src->width(), false);
	image<float> *dst = new image<float>(src->width(), src->height(), false);
	convolve_even(src, tmp, mask);
	convolve_even(tmp, dst, mask);

	delete tmp;
	return dst;
}


// compute laplacian 
static image<float> *laplacian(image<float> *src) {
	int width = src->width();
	int height = src->height();
	image<float> *dst = new image<float>(width, height);

	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			float d2x = imRef(src, x - 1, y) + imRef(src, x + 1, y) -
				2 * imRef(src, x, y);
			float d2y = imRef(src, x, y - 1) + imRef(src, x, y + 1) -
				2 * imRef(src, x, y);
			imRef(dst, x, y) = d2x + d2y;
		}
	}
	return dst;
}

#endif
