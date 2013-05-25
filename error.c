
#include "error.h"

static char *error_string = 0;
static int error_position = 0;

char *get_err_str()
{
	return error_string;
}

int get_err_pos()
{
	return error_position;
}

int set_err_str(char *what)
{
	error_string = what;
	return 0;
}

int set_err_pos(int pos)
{
	error_position = pos;
	return 0;
}

int set_err_str_pos(char *what, int pos)
{
	error_string = what;
	error_position = pos;
	return 0;
}

