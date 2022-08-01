#include <unistd.h>
#include "../../prototype/libraries/Libft/libft.h"
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char **argv, char **envp)
{
	char *limiter;
	char *buf;
	int rd;
	int fd;

	buf = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	fd = open("outfile", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	limiter = ft_strdup(argv[1]);
	rd = 1;
	dup2(fd, STDOUT_FILENO);
	while (rd > 0)
	{
		rd = read(STDIN_FILENO, buf, BUFFER_SIZE);
		buf[rd] = '\0';
		if (ft_strnstr(buf, limiter, ft_strlen(limiter)))
			break;
		ft_printf("%s",buf);
	}
	free(limiter);
	free(buf);
	close(fd);
}
