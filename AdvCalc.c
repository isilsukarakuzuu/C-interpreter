#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "variables.h"
bool error = false;
const char *operators = "+-*&|";
char *reserved_functions[] = {"xor", "not", "ls", "rs", "lr", "rr"};
// xor --> ^, not --> ~, ls --> <, rs --> >, lr --> [, rr --> ]
const char *new_operators = "^~<>[]";

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
    if (input[i] == ' ')
    {
      continue;
    }
    // check if the character is in given another set of valid characters
    char *valid_chars = "()%,+*-&|=";
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
      input[i] = '\0';
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
    if (input[i] == ' ')
    {

      if (!(isalpha(input[i - 1]) || isdigit(input[i - 1])))
      {
        continue;
      }
      while (input[i] == ' ')
      {
        i++;
      }
      if (!(isalpha(input[i]) || isdigit(input[i])))
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
    if (input[i] != ' ')
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
    if (!strncmp(input, reserved_functions[i], length))
    {
      return 0;
    }
  }
  return 1;
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
    return length;
  }
  return -1;
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
      printf("function: %s\n", function);

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
        printf("error: invalid function call\n");
        error = true;
        continue;
      }
      // it replaces the comma with assigned new function operator
      input[comma_index] = new_operators[k];
    }
  }
}

long long int expression_value_finder(char *input, int length)
{
  if (length == 0)
  {
    error = true;
  }
  // check whether the input's first or last char is an operator
  if (strchr(operators, input[0]) != NULL || strchr(operators, input[length - 1]) != NULL)
  {
    error = true;
  }

  // check whether the input has two operators next to each other
  for (int i = 0; i < length - 1; i++)
  {
    if (strchr(operators, input[i]) != NULL && strchr(operators, input[i + 1]) != NULL)
    {
      error = true;
    }
  }
  // print the input
  for (int i = 0; i < length; i++)
  {
    // printf("%c", input[i]);
  }
  // printf("\n");
  return 5;
}

long long int expression_parser(char *input, int length)
{
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
      sprintf(sub_expr + k, " %lld ", expression);
      // increase k by the number of digits in the expression + 2
      while (expression != 0)
      {
        expression /= 10;
        k++;
      }
      k += 2;
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
    error = 1;
  }
  space_deleter(sub_expr);
  long long int temp = expression_value_finder(sub_expr, strlen(sub_expr));
  free(sub_expr);
  return temp;
}
// TODO: it must assign the functions to the reserved operators
// TODO: it must check if the input has valid parentheses ( and ) must be equal and must be in the correct places
// TODO: it must check if the input has no reserved word variables

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

    // checks if the input contains only valid characters
    if (!contains_valid_chars(input))
    {
      printf("Error!\n");
      continue;
    }

    // checks if the input is a comment
    is_it_a_comment(input);

    // checks if the spaces are placed correctly
    if (!are_spaces_placed_correctly(input))
    {
      printf("Error!\n");
      continue;
    }

    // deletes all the spaces in the input
    space_deleter(input);

    // checks the parantheses briefly
    if (!are_parantheses_placed_correctly(input))
    {
      printf("Error!\n");
      continue;
    }

    // checks if the input is a equation
    if (is_it_a_equation(input) != -1)
    {
      int length = is_it_a_equation(input);
      // checks the left side of the equal sign, it must be a variable
      if (!variable_checker(input, length))
      {
        printf("Error!\n");
        continue;
      }
      function_parser(input);
      // space_deleter(input);
      //  TODO: it must calculate the expression on the right side of the equal sign and assign it to the variable on the left side of the equal sign
      long long int result = expression_parser(input + length + 1, strlen(input) - length - 1);
      if (error)
      {
        printf("Error!\n");
        error = false;
        continue;
      }
      printf("%d result\n", result);
      char *variable = malloc(length);
      strncpy(variable, input, length - 1);
      variable[length - 1] = '\0';
      set_variable(input, result);
      continue;
    }

    function_parser(input);
    space_deleter(input);
    for (int q = 0; input[q] != '\0'; q++)
    {
      printf("%c", input[q]);
    }
    printf("\n");
    // TODO: it must calculate the expression and print the result
    long long int result = expression_parser(input, strlen(input));

    if (error)
    {
      printf("Error!\n");
      error = false;
      continue;
    }
    printf("%d result\n", result);
  }

  // free the variables
  free_variables();
}