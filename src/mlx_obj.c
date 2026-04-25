/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_obj.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/21 11:33:47 by kong              #+#    #+#             */
/*   Updated: 2026/04/21 11:33:49 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "fdf.h"

/*
https://gontjarow.github.io/MiniLibX/mlx-tutorial-create-image.html
The “proper” way to draw the image according to the manual is to:

1. Check how many bits there are per pixel --> how?
2. Convert your color with mlx_get_color_value if necessary.
3. Check whether the environment is little/big endian. --> how?
4. Write your color value byte-by-byte into the pixel array, according to endianness.
	- this pixel array is returned by `mlx_get_data_addr`
*/

t_mlx	*setup_mlx()
{
	t_mlx	*mlx_obj;
	void	*conn_ptr;
	void	*win_ptr;

	mlx_obj = malloc(sizeof(mlx_obj));
	mlx_obj->conn_ptr = mlx_init();
	if (!mlx_obj->conn_ptr)
		return (EXIT_FAILURE);
	mlx_obj->win_ptr = mlx_new_window(conn_ptr, WIN_WIDTH, WIN_HEIGHT, "FdF");
	if (!mlx_obj->win_ptr)
		return (freemlx(mlx_obj), EXIT_FAILURE);

	return (mlx_obj);
}
