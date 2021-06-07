#include "simlib.h"
#include "normal.h"

double normal_distribution(double mean, double standard_dev, int s1, int s2) {
  while (1) {
    double v1 = double(2.0 * lcgrand(s1) - 1.0);
    double v2 = double(2.0 * lcgrand(s2) - 1.0);
    double w = v1 * v1 + v2 * v2;
    if (w <= 1) {
      double y = sqrt(-2.0 * log(w) / w);
      return (v1 * y) * standard_dev + mean;
    }
  }
}

double truncated_normal(double mean, double standard_dev, double min_value, double max_value, int s1, int s2) {
  while (1) {
    double sample = normal_distribution(mean, standard_dev, s1, s2);
    if (sample >= min_value && sample <= max_value)
      return sample;
  }
}
