
#ifndef ERROR_H
#define ERROR_H

char *get_err_str();
int get_err_pos();
int set_err_str(char *what);
int set_err_pos(int pos);
int set_err_str_pos(char *what, int pos);

#endif
