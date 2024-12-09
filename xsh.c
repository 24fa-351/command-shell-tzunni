#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "xsh.h"

#define MAX_ARG_SIZE 100

void parse_and_execute(char *input)
{
  char *args[MAX_ARG_SIZE];
  char *token = strtok(input, " \t\r\n");
  int arg_count = 0;
  int background = 0;
  int input_fd = -1;
  int output_fd = -1;

  while (token != NULL && arg_count < MAX_ARG_SIZE - 1)
  {
    if (strcmp(token, "&") == 0)
    {
      background = 1;
    }
    else if (strcmp(token, "<") == 0)
    {
      token = strtok(NULL, " \t\r\n");
      if (token == NULL)
      {
        fprintf(stderr, "xsh: expected file after '<'\n");
        return;
      }
      input_fd = open(token, O_RDONLY);
      if (input_fd < 0)
      {
        perror("xsh");
        return;
      }
    }
    else if (strcmp(token, ">") == 0)
    {
      token = strtok(NULL, " \t\r\n");
      if (token == NULL)
      {
        fprintf(stderr, "xsh: expected file after '>'\n");
        return;
      }
      output_fd = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (output_fd < 0)
      {
        perror("xsh");
        return;
      }
    }
    else
    {
      args[arg_count++] = token;
    }
    token = strtok(NULL, " \t\r\n");
  }
  args[arg_count] = NULL;

  pid_t pid = fork();
  if (pid == 0)
  {
    if (input_fd != -1)
    {
      dup2(input_fd, STDIN_FILENO);
      close(input_fd);
    }
    if (output_fd != -1)
    {
      dup2(output_fd, STDOUT_FILENO);
      close(output_fd);
    }
    execvp(args[0], args);
    perror("xsh");
    exit(EXIT_FAILURE);
  }
  else if (pid > 0)
  {
    if (!background)
    {
      waitpid(pid, NULL, 0);
    }
  }
  else
  {
    perror("xsh");
  }
}

void handle_cd(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "xsh: expected argument to \"cd\"\n");
  }
  else
  {
    if (chdir(args[1]) != 0)
    {
      perror("xsh");
    }
  }
}

void handle_pwd()
{
  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
  {
    printf("%s\n", cwd);
  }
  else
  {
    perror("xsh");
  }
}

void handle_set(char **args)
{
  if (args[1] == NULL || args[2] == NULL)
  {
    fprintf(stderr, "xsh: expected arguments to \"set\"\n");
  }
  else
  {
    setenv(args[1], args[2], 1);
  }
}

void handle_unset(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "xsh: expected argument to \"unset\"\n");
  }
  else
  {
    unsetenv(args[1]);
  }
}

void handle_echo(char **args)
{
  for (int i = 1; args[i] != NULL; i++)
  {
    if (args[i][0] == '$')
    {
      char *env_var = getenv(args[i] + 1);
      if (env_var != NULL)
      {
        printf("%s ", env_var);
      }
    }
    else
    {
      printf("%s ", args[i]);
    }
  }
  printf("\n");
}

void execute_external_command(char **args, int background, int input_fd, int output_fd)
{
  pid_t pid = fork();
  if (pid == 0)
  {
    if (input_fd != -1)
    {
      dup2(input_fd, STDIN_FILENO);
      close(input_fd);
    }
    if (output_fd != -1)
    {
      dup2(output_fd, STDOUT_FILENO);
      close(output_fd);
    }
    if (execvp(args[0], args) == -1)
    {
      perror("xsh");
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    perror("xsh");
  }
  else
  {
    if (!background)
    {
      int status;
      waitpid(pid, &status, 0);
    }
  }
}
