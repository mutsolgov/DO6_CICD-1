#ifndef S21_CAT_H_
#define s21_CAT_H_

typedef struct argmunets {
  int b, n, s, E, T, v;
} argmunets;

argmunets argumet_parser(int argc, char** argv);
void outline(argmunets* arg, char* line, int n);
void output(argmunets* arg, char** argv);

#endif