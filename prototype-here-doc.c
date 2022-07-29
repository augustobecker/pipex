#include <unistd.h>
#include "../../prototype/libraries/Libft/libft.h"
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFER_SIZE 10

int main(int argc, char **argv, char **envp)
{
	char *limiter;
	char *buf;
	char *temp_buf;
	int rd;
	int doc_size;

	buf = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	limiter = ft_strdup(argv[1]);
	rd = read(STDIN_FILENO, buf, BUFFER_SIZE);
	buf[rd] = '\0';
	temp_buf = ft_strdup("");
	while (rd > 0)
	{
		temp_buf = ft_strjoin(temp_buf, buf);
		if (ft_strnstr(temp_buf, limiter, ft_strlen(temp_buf)))
            break ;
        rd = read(STDIN_FILENO, buf, BUFFER_SIZE);
		buf[rd] = '\0';
	}
	doc_size = ft_find_limiter(temp_buf, limiter, ft_strlen(temp_buf));
	temp_buf = ft_substr(temp_buf, 0, doc_size);
	ft_printf("%s\n", temp_buf);
}
