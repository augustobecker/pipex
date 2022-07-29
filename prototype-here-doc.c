#include <unistd.h>
#include "../../prototype/libraries/Libft/libft.h"
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 10

int main(int argc, char **argv, char **envp)
{
	char *doc_temp;
	char *limiter;
	char *buf;
	char *temp_buf;
	int rd;

	buf = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	limiter = ft_strdup(argv[1]);
	rd = read(STDIN_FILENO, buf, BUFFER_SIZE);
	temp_buf = ft_strdup(buf);
	while (rd > 0)
	{
		buf[rd] = '\0';
		if (!ft_strncmp(buf, limiter, ft_strlen(limiter)))
            break ;
        rd = read(STDIN_FILENO, buf, BUFFER_SIZE);
	}
	ft_printf("%s\n", buf);
}
