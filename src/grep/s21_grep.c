#include "s21_grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
  arguments arg = arguments_parser(argc, argv);
  output(arg, argc, argv);
  return 0;
}

void pattern_add(arguments* arg, char* pattern) {
  if (arg->len_pattern != 0) {
    strcat(arg->pattern + arg->len_pattern, "|");
    arg->len_pattern++;
  }
  arg->len_pattern += sprintf(arg->pattern + arg->len_pattern, "(%s)", pattern);
}

void add_reg_from_file(arguments* arg, char* filepath) {
  FILE* file = fopen(filepath, "r");
  if (file == NULL) {
    if (!arg->s) perror(filepath);
    exit(1);
  }
  char* line = NULL;
  size_t len = 0;
  int read = getline(&line, &len, file);
  while (read != -1) {
    if (line[read - 1] == '\n') line[read - 1] = '\0';
    pattern_add(arg, line);
    read = getline(&line, &len, file);
  }
  free(line);
  fclose(file);
}

arguments arguments_parser(int argc, char* argv[]) {
  arguments arg = {0};
  int opt;
  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (opt) {
      case 'e':
        arg.e = 1;
        pattern_add(&arg, optarg);
        break;
      case 'i':
        arg.i = REG_ICASE;
        break;
      case 'v':
        arg.v = 1;
        break;
      case 'c':
        arg.c = 1;
        break;
      case 'l':
        arg.c = 1;
        arg.l = 1;
        break;
      case 'n':
        arg.n = 1;
        break;
      case 'h':
        arg.h = 1;
        break;
      case 's':
        arg.s = 1;
        break;
      case 'f':
        arg.f = 1;
        add_reg_from_file(&arg, optarg);
        break;
      case 'o':
        arg.o = 1;
        break;
    }
  }
  if (arg.len_pattern == 0) {
    pattern_add(&arg, argv[optind]);
    optind++;
  }
  if (argc - optind == 1) {
    arg.h = 1;
  }
  return arg;
}

void output_line(char* line, int length) {
  fwrite(line, sizeof(char), length, stdout);
  if (line[length - 1] != '\n') putchar('\n');
}

void print_match(regex_t* re, char* line) {
  regmatch_t match;
  int offset = 0;
  while (regexec(re, line + offset, 1, &match, 0) == 0) {
    fwrite(line + offset + match.rm_so, sizeof(char), match.rm_eo - match.rm_so,
           stdout);
    putchar('\n');
    offset += match.rm_eo;
  }
}

void processFile(arguments arg, char* path, regex_t* reg) {
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    if (!arg.s) perror(path);
    exit(1);
  }
  char* line = NULL;
  size_t len = 0;
  int read = 0;
  int line_count = 1;
  int c = 0;
  read = getline(&line, &len, file);
  while (read != -1) {
    int result = regexec(reg, line, 0, NULL, 0);

    if ((result == 0 && !arg.v) || (arg.v && result != 0)) {
      if (!arg.c && !arg.l) {
        if (!arg.h) printf("%s:", path);
        if (arg.n) printf("%d:", line_count);
        if (arg.o) {
          print_match(reg, line);
        } else {
          output_line(line, read);
        }
      }
      c++;
    }
    read = getline(&line, &len, file);
    line_count++;
  }
  free(line);
  if (arg.c && !arg.l) {
    if (!arg.h) printf("%s:", path);
    printf("%d\n", c);
  }
  if (arg.l && c > 0) printf("%s\n", path);
  fclose(file);
}

void output(arguments arg, int argc, char** argv) {
  regex_t re;
  int error = regcomp(&re, arg.pattern, REG_EXTENDED | arg.i);
  if (error) perror("Error");
  for (int i = optind; i < argc; i++) {
    processFile(arg, argv[i], &re);
  }
  regfree(&re);
}
