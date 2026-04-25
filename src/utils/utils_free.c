/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_free.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/10 15:19:44 by kong              #+#    #+#             */
/*   Updated: 2026/04/24 10:38:22 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void	freelst(void **lst)
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

void	freemlx(t_mlx *mlx)
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
		free(mlx);
	}
}

// ! check again
int	freeprog(t_data *prog, int code)
{
	if (prog->mlx_obj)
		destroy_mlx(prog->mlx_obj);
	if (prog->map_obj)
	{
		freelst(prog->map_obj->src_lst);
		free(prog->map_obj);
	}
	if (prog->vp_obj)
		...;
	if (prog->img_obj)
		...;
	free(prog);
}
