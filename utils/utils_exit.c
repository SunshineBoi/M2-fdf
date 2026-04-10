/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_exit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:49:32 by kong              #+#    #+#             */
/*   Updated: 2026/04/10 18:12:43 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void	exitprog(t_mlx *mlx, int code)
{
	if (mlx)
	{
		if (mlx->win_ptr)
		{
			mlx_destroy_window(mlx->conn_ptr, mlx->win_ptr);
		}
		if (mlx->conn_ptr)
		{
			mlx_destroy_display(mlx->conn_ptr);
			free(mlx->conn_ptr);
		}
	}
	// ! note: t_prog should lives in stack
	exit(code);
}

// void	input_errexit(char **lst)
// {
// 	if (lst)
// 		free_list(lst);
// 	ft_putstr_fd("Error\n", 2);
// 	exit(1);
// }

// sys error exit
void	perror_exit(char *msg, int code)
{
	perror(msg);
	exit(code);
}

void	errexit(char *msg, int code)
{
	if (msg)
	{
		ft_putstr_fd(msg, 2);
		ft_putchar_fd('\n', 2);
	}
	exit(code);
}

void	free_lst_and_exit(char **lst, char *msg, int code)
{
	freelst(lst);
	errexit(msg, code);
}
