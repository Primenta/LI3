/**
 * @file interpreter.h
 * @brief Header file for the interpreter module.
*/
#ifndef INTERPRETER_H
#define INTERPRETER_H

void command_interpreter(char *input, char **res);
void free_command(char **res);

#endif