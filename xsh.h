void execute_command(char *cmd);
void handle_cd(char **args);
void handle_pwd();
void handle_set(char **args);
void handle_unset(char **args);
void handle_echo(char **args);
void execute_external_command(char **args, int background, int input_fd, int output_fd);
void parse_and_execute(char *input);
