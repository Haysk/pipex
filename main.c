/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adylewsk <adylewsk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/08 16:56:41 by adylewsk          #+#    #+#             */
/*   Updated: 2021/11/09 01:16:01 by adylewsk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.h"

void	ft_error(int error, char *str)
{
	ft_putstr_fd(str, STDERR_FILENO);
	ft_putstr_fd(" : ", STDERR_FILENO);
	ft_putstr_fd(strerror(error), STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
	exit(EXIT_FAILURE);
}

int	process_one(char **argv, int *end, char **envp)
{
	int		fd;
	char	**args;
	char	*cmd;

	fd = open(argv[0], O_RDONLY);
	if (fd < 0)
		ft_error(errno, argv[0]);
	if (dup2(fd, STDIN_FILENO) < 0 || dup2(end[1], STDOUT_FILENO) < 0)
		ft_error(errno, "process one -> dup2");
	close(fd);
	close(end[0]);
	args = ft_split(argv[1], ' ');
	cmd = check_exe(args[0], envp);
	execve(cmd, args, envp);
	if (cmd)
		free(cmd);
	ft_freetab(args);
	ft_error(errno, argv[1]);
	return (0);
}

int	process_two(char **argv, int *end, char **envp)
{
	int		fd;
	char	**args;
	char	*cmd;

	fd = open(argv[3], O_CREAT | O_RDWR | O_TRUNC, 00664);
	if (fd < 0)
		ft_error(errno, argv[3]);
	if (dup2(fd, STDOUT_FILENO) < 0 || dup2(end[0], STDIN_FILENO) < 0)
		ft_error(errno, "process two -> dup2");
	close(fd);
	close(end[1]);
	args = ft_split(argv[2], ' ');
	cmd = check_exe(args[0], envp);
	execve(cmd, args, envp);
	if (cmd)
		free(cmd);
	ft_freetab(args);
	ft_error(errno, argv[2]);
	return (0);
}

int	pipex(char **argv, char **envp)
{
	int		status;
	int		end[2];
	pid_t	p_one;
	pid_t	p_two;

	errno = 0;
	if (pipe(end) == -1)
		ft_error(errno, "pipe");
	p_one = fork();
	if (p_one == 0)
		process_one(argv, end, envp);
	else if (p_one == -1)
		ft_error(errno, "process one");
	p_two = fork();
	close(end[1]);
	if (p_two == 0)
		process_two(argv, end, envp);
	else if (p_two == -1)
		ft_error(errno, "process two");
	close(end[0]);
	close(end[1]);
	wait(&status);
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	if (argc != 5)
		exit(EXIT_FAILURE);
	pipex(argv + 1, envp);
	return (0);
}
