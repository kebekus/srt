
#ifndef EVAL_H
#define EVAL_H
float func_pow(float left, float right);
float func_mul(float left, float right);
float func_div(float left, float right);
float func_add(float left, float right);
float func_sub(float left, float right);
float func_neg(float left, float right);
float func_sqrt(float left, float right);
float func_sin(float left, float right);
float func_cos(float left, float right);
float parser_eval_branch(struct parser_node *node, float x, float y, float z, float a);
float parser_eval(struct parser_tree *tree, float x, float y, float z, float a);
#endif
