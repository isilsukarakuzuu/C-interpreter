#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "variables.h"

bool debug = false;
bool error = false;
const char *operators = "+-*&|";
char *reserved_functions[] = {"xor", "not", "ls", "rs", "lr", "rr"};
// xor --> ^, ls --> <, rs --> >, lr --> [, rr --> ]
const char *new_operators = "^~<>[]";
int temp_count = 10000;

// TODO: check if the memory is allocated correctly and freed correctly
// TODO: check piazza for more test cases
// TODO: run in linux

void debug_printer(char *error_message)
{
  if (debug)
  {
    printf("error:%s\n", error_message);
  }
}

void array_printer(char *array, int length)
{
  if (debug)
  {
    for (int i = 0; i < length; i++)
    {
      printf("%c ", array[i]);
    }
    printf("\n");
  }
}

int index_finder(char *input, int length, char *operator_array)
{
  // returns the index of the first operator in the operator array
  for (int i = 0; i < length; i++)
  {
    for (int j = 0; operator_array[j] != '\0'; j++)
    {
      if (input[i] == operator_array[j])
      {
        return i;
      }
    }
  }
  return -1;
}

int contains_valid_chars(char *input)
{
  int i = 0;
  for (i = 0; input[i] != '\0'; i++)
  {
    // checks if the character is a digit
    if (input[i] >= '0' && input[i] <= '9')
    {
      continue;
    }
    // check if the character is lowercase letter
    if (input[i] >= 'a' && input[i] <= 'z')
    {
      continue;
    }
    // check if the character is uppercase letter
    if (input[i] >= 'A' && input[i] <= 'Z')
    {
      continue;
    }
    // check if the character is a space
    if (input[i] == ' ' || input[i] == '\t')
    {
      continue;
    }
    // check if the character is in given another set of valid characters
    char *valid_chars = "(),+*-&|=";
    if (strchr(valid_chars, input[i]) != NULL)
    {
      continue;
    }
    return 0;
  }
  return 1;
}

int is_it_a_comment(char *input)
{
  // checks if the input starts with a %
  for (int i = 0; input[i] != '\0'; i++)
  {
    if (input[i] == '%')
    {
      while (input[i] != '\0')
      {
        input[i] = ' ';
        i++;
      }
      return 1;
    }
  }
  return 0;
}

int are_spaces_placed_correctly(char *input)
{
  // checks if the input has spaces in the correct places
  for (int i = 1; input[i] != '\0'; i++)
  {
    if (input[i] == ' ' || input[i] == '\t')
    {

      if (!(isalpha(input[i - 1]) || isdigit(input[i - 1])))
      {
        continue;
      }
      while ((input[i] == ' ' || input[i] == '\t') && input[i] != '\0')
      {
        i++;
      }
      if (!(isalpha(input[i]) || isdigit(input[i]) || input[i] == '_'))
      {
        continue;
      }
      return 0;
    }
  }
  return 1;
}

int space_deleter(char *input)
{
  // deletes all the spaces in the input
  int i = 0;
  int j = 0;
  while (input[i] != '\0')
  {
    if (input[i] != ' ' && input[i] != '\t')
    {
      input[j] = input[i];
      j++;
    }
    i++;
  }
  input[j] = '\0';
  return 1;
}

int variable_checker(char *input, int length)
{
  // checks if the input is not empty
  if (length == 0)
  {
    return 0;
  }

  if (input[0] == '_')
  {
    return 1;
  }

  // checks if the input has only uppercase and lowercase letters
  for (int i = 0; i < length; i++)
  {
    if (input[i] >= 'a' && input[i] <= 'z')
    {
      continue;
    }
    if (input[i] >= 'A' && input[i] <= 'Z')
    {
      continue;
    }
    return 0;
  }

  // checks if the input is not exactly one of the reserved words
  for (int i = 0; i < 6; i++)
  {
    char *var = (char *)malloc(sizeof(char) * (length + 1));
    var[length] = '\0';

    // copy input to var
    for (int j = 0; j < length; j++)
    {
      var[j] = input[j];
    }

    if (!strcmp(var, reserved_functions[i]))
    {
      free(var);
      return 0;
    }
    free(var);
  }
  return 1;
}

long long int value_checker(char *input, int length)
{
  // checks if the input is not empty
  if (length == 0)
  {
    return -1;
  }

  // checks if the input has only digits
  for (int i = 0; i < length; i++)
  {
    if (input[i] >= '0' && input[i] <= '9')
    {
      continue;
    }
    return -1;
  }

  // checks if the input is not too long
  if (length > 19)
  {
    return -1;
  }

  // checks if the input is not too big
  long long int value = 0;
  for (int i = 0; i < length; i++)
  {
    value = value * 10 + (input[i] - '0');
  }
  if (value > 9223372036854775807ll)
  {
    return -1;
  }
  return value;
}

int is_it_a_equation(char *input)
{
  // checks if the input has only 1 equal sign
  int count = 0;
  int length = 0;
  for (int i = 0; input[i] != '\0'; i++)
  {
    if (input[i] == '=')
    {
      length = i;
      count++;
    }
  }
  if (count == 1)
  {
    if (length == 0)
    {
      debug_printer("No variable name before the equal sign.");
      error = true;
      return -1;
    }
    return length;
  }
  if (count > 1)
  {
    debug_printer("More than one equal sign.");
    error = true;
    return -1;
  }
  return 0;
}

int are_parantheses_placed_correctly(char *input)
{
  // checks if the input has equal number of ( and )
  int count = 0;
  for (int i = 0; input[i] != '\0'; i++)
  {
    if (input[i] == '(')
    {
      count++;
    }
    if (input[i] == ')')
    {
      count--;
      if (count < 0)
      {
        return 0;
      }
    }
  }
  if (count == 0)
  {
    return 1;
  }
  return 0;
}

int function_parser(char *input)
{
  for (int i = 0; input[i] != '\0'; i++)
  {

    if (input[i] != '(')
      continue;

    // itarates the string backwards till its not uppercase or lowercase letter, then it checks the final string is in the reserved functions
    int j = i - 1;
    while (j >= 0 && ((input[j] >= 'a' && input[j] <= 'z') || (input[j] >= 'A' && input[j] <= 'Z')))
    {
      j--;
    }
    j++;
    char *function = malloc(sizeof(char) * (i - j + 1));
    for (int k = 0; k < i - j; k++)
    {
      function[k] = input[j + k];
    }
    function[i - j] = '\0';
    for (int k = 0; k < 6; k++)
    {
      if (strcmp(function, reserved_functions[k]))
        continue;

      // it replaces the function name chars with space from actual string
      for (int l = j; l < i; l++)
      {
        input[l] = ' ';
      }
      if (k == 1)
      {
        input[i - 1] = '!';
        continue;
      }
      int level = 0;
      int m = i;
      int comma_count = 0;
      int comma_index = -1;
      do
      {
        if (input[m] == '(')
        {
          level++;
        }
        if (input[m] == ')')
        {
          level--;
        }
        if (input[m] == ',' && level == 1)
        {
          comma_count++;
          comma_index = m;
        }
        m++;
      } while (level != 0);
      if (comma_count > 1 || comma_index == -1)
      {
        debug_printer("Invalid number of commas.");
        error = true;
        continue;
      }
      // it replaces the comma with assigned new function operator
      input[comma_index] = new_operators[k];
    }
    free(function);
  }
}

long long int expression_value_finder(char *input, int length)
{
  // debug_printer("expression_value_finder called.");
  // array_printer(input, length);
  if (length == 0)
  {
    debug_printer("Empty input in exp value finder.");
    error = true;
    return 0;
  }
  // check whether the input's first or last char is an operator
  if (strchr(operators, input[0]) != NULL || strchr(operators, input[length - 1]) != NULL)
  {
    debug_printer("Input starts or ends with an operator.");
    error = true;
  }

  // check whether the input has two operators next to each other
  for (int i = 0; i < length - 1; i++)
  {
    if (strchr(operators, input[i]) != NULL && strchr(operators, input[i + 1]) != NULL)
    {
      debug_printer("Two operators next to each other.");
      error = true;
    }
  }
  int index = -1;
  // new operators
  index = index_finder(input, length, "^~<>[]");

  if (index != -1)
  {
    // xor --> ^, not --> ~, ls --> <, rs --> >, lr --> [, rr --> ]
    if (input[index] == '^')
    {
      return expression_value_finder(input, index) ^ expression_value_finder(input + index + 1, length - index - 1);
    }
    if (input[index] == '<')
    {
      return expression_value_finder(input, index) << expression_value_finder(input + index + 1, length - index - 1);
    }
    if (input[index] == '>')
    {
      return expression_value_finder(input, index) >> expression_value_finder(input + index + 1, length - index - 1);
    }
    if (input[index] == '[')
    {
      // Returns the result of a rotated i times to the left.
      long long int a = expression_value_finder(input, index);
      return (a << expression_value_finder(input + index + 1, length - index - 1)) | (a >> (64 - expression_value_finder(input + index + 1, length - index - 1)));
    }
    if (input[index] == ']')
    {
      // Returns the result of a rotated i times to the right.
      long long int a = expression_value_finder(input, index);
      return (a >> expression_value_finder(input + index + 1, length - index - 1)) | (a << (64 - expression_value_finder(input + index + 1, length - index - 1)));
    }
  }

  // | operator
  index = index_finder(input, length, "|");

  if (index != -1)
  {
    return expression_value_finder(input, index) | expression_value_finder(input + index + 1, length - index - 1);
  }
  // & operator
  index = index_finder(input, length, "&");

  if (index != -1)
  {
    return expression_value_finder(input, index) & expression_value_finder(input + index + 1, length - index - 1);
  }

  // +-* operators
  index = index_finder(input, length, "+-");

  if (index != -1)
  {
    if (input[index] == '+')
    {
      return expression_value_finder(input, index) + expression_value_finder(input + index + 1, length - index - 1);
    }
    else
    {
      return expression_value_finder(input, index) - expression_value_finder(input + index + 1, length - index - 1);
    }
  }

  index = index_finder(input, length, "*");

  if (index != -1)
  {
    return expression_value_finder(input, index) * expression_value_finder(input + index + 1, length - index - 1);
  }

  if (input[0] == '!')
  {
    return ~expression_value_finder(input + 1, length - 1);
  }

  long long int value = value_checker(input, length);
  if (value != -1)
  {
    return value;
  }

  if (variable_checker(input, length))
  {
    return variable_value(input, length);
  }
  debug_printer("Invalid exp value finder.");
  printf("Invalid expression: ");
  array_printer(input, length);
  printf("%d", temp_count);
  error = true;
  return 0;
}

long long int expression_parser(char *input, int length)
{
  // debug_printer("expression_parser called.");
  // array_printer(input, length);
  char *sub_expr = (char *)malloc(6 * length * sizeof(char));
  int k = 0;
  int level = 0;
  for (int i = 0; i < length; i++)
  {
    if (input[i] == '(')
    {
      int j = i;
      level++;
      while (level != 0)
      {
        j++;
        if (input[j] == '(')
        {
          level++;
        }
        if (input[j] == ')')
        {
          level--;
        }
      }

      long long int expression = expression_parser(input + i + 1, (j - 1) - (i + 1) + 1);
      // change temp_count to a string and set it as a variable

      char *temp = (char *)malloc(7 * sizeof(char));
      temp[6] = '\0';
      sprintf(temp, "_%d", temp_count);
      temp_count++;

      set_variable(temp, expression);
      sprintf(sub_expr + k, " %s ", temp);

      // increase k by the number of digits in the expression + 2
      k += strlen(temp) + 2;
      i = j;
    }
    else
    {
      sub_expr[k] = input[i];
      k++;
    }
  }
  sub_expr[k] = '\0';
  if (!are_spaces_placed_correctly(sub_expr))
  {
    debug_printer("spaces are not placed correctly");
    error = 1;
  }
  space_deleter(sub_expr);
  long long int temp = expression_value_finder(sub_expr, strlen(sub_expr));
  // printf("input: %s\n", sub_expr);
  // printf("temp: %lld\n", temp);

  free(sub_expr);
  return temp;
}

int main()
{
  char input[257];
  while (true)
  {
    printf("> ");
    if (fgets(input, sizeof(input), stdin) == NULL)
    {
      break;
    }

    input[strcspn(input, "\n")] = '\0';
    // check whether input is new line
    if (input[0] == '\0')
    {
      continue;
    }

    int space_check = 0;
    // check whether input consist only spaces
    for (int i = 0; i < strlen(input); i++)
    {
      if (input[i] == ' ' || input[i] == '\t' || input[i] == '\n')
      {
        space_check++;
      }
    }
    if (space_check == strlen(input))
    {
      continue;
    }

    // checks if the input is a comment
    is_it_a_comment(input);

    // checks if the input contains only valid characters
    if (!contains_valid_chars(input))
    {
      debug_printer("contains invalid chars");
      printf("Error!\n");
      continue;
    }

    // checks if the spaces are placed correctly
    if (!are_spaces_placed_correctly(input))
    {
      debug_printer("spaces are not placed correctly");
      array_printer(input, strlen(input));
      printf("Error!\n");
      continue;
    }

    // deletes all the spaces in the input
    space_deleter(input);

    // checks the parantheses briefly
    if (!are_parantheses_placed_correctly(input))
    {
      debug_printer("parantheses are not placed correctly");
      printf("Error!\n");
      continue;
    }

    int length = is_it_a_equation(input);
    if (error)
    {
      printf("Error!\n");
      error = false;
      continue;
    }

    // checks if the input is a equation
    if (length >= 1)
    {
      // checks the left side of the equal sign, it must be a variable
      if (!variable_checker(input, length))
      {
        debug_printer("left side of the equal sign is not a variable");
        printf("Error!\n");
        continue;
      }
      function_parser(input);
      space_deleter(input);

      //  TODO: it must calculate the expression on the right side of the equal sign and assign it to the variable on the left side of the equal sign
      long long int result = expression_parser(input + length + 1, strlen(input) - length - 1);
      if (error)
      {
        printf("Error!\n");
        error = false;
        continue;
      }
      char *variable = malloc(length + 1);
      strncpy(variable, input, length);
      variable[length] = '\0';
      set_variable(variable, result);
      continue;
    }

    function_parser(input);
    space_deleter(input);

    // TODO: it must calculate the expression and print the result
    long long int result = expression_parser(input, strlen(input));

    if (error)
    {
      printf("Error!\n");
      error = false;
      continue;
    }
    printf("%lld\n", result);
  }

  // free the variables
  free_variables();
}