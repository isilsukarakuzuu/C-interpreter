// implement a list for variables, it must hold keys as variables and values as their values

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct variable
{
  char *name;
  long long int value;
};

struct variable *variables[256];
int variable_count = 0;

int variable_value(char *name, int length)
{
  for (int i = 0; i < variable_count; i++)
  {

    if (strncmp(variables[i]->name, name, length) == 0)
    {
      return variables[i]->value;
    }
  }
  return 0;
}

void set_variable(char *name, long long int value)
{
  for (int i = 0; i < variable_count; i++)
  {
    if (strcmp(variables[i]->name, name) == 0)
    {
      variables[i]->value = value;
      free(name);
      return;
    }
  }
  variables[variable_count] = (struct variable *)malloc(sizeof(struct variable));
  variables[variable_count]->name = name;
  variables[variable_count]->value = value;
  variable_count++;
}

void free_variables()
{
  for (int i = 0; i < variable_count; i++)
  {
    free(variables[i]);
  }
}
