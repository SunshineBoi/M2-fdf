/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:27 by kong              #+#    #+#             */
/*   Updated: 2026/04/09 00:22:46 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

// cc src/main.c -Iinclude -Iminilibx-linux minilibx-linux/libmlx.a -lXext -lX11 -lm
int main(void)
{
	void *mlx;
	void *mlx_winm;

	mlx = mlx_init();
	if (!mlx)
		return (1);
	mlx_winm = mlx_new_window(mlx, 500, 500, "hello window!");
	mlx_pixel_put(mlx, mlx_winm, 250, 250, rand() % 0x1000000);
	mlx_loop(mlx);

	mlx_new_image()

	mlx_destroy_display(mlx); // because mlx_init malloc a display
	free(mlx);
	return (0);
}
