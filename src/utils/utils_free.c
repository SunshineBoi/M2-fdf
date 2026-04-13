/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_free.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 15:19:44 by kong              #+#    #+#             */
/*   Updated: 2026/04/13 17:25:55 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void	destroy_mlx(t_mlx *mlx)
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
}

void	free_lst(void **lst)
{
	int	i;

	if (lst)
	{
		i = 0;
		while (lst[i])
		{
			free(lst[i]);
			i++;
		}
		free(lst);
	}
}

// ! check again
void	free_prog(t_prog *prog)
{
	if (prog->map_obj)
	{
		free_lst(prog->map_obj->coords_lst);
		free(prog->map_obj);
	}
	if (prog->mlx_obj)
		destroy_mlx(prog->mlx_obj);
	if (prog->vp_obj)
		...;
	free(prog);
}
