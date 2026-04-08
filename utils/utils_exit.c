/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_exit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:49:32 by kong              #+#    #+#             */
/*   Updated: 2026/04/08 12:58:12 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void	exitprog(t_prog *prog)
{
	if (prog)
	{
		if (prog->mlx_win)
		{
			mlx_destroy_window(prog->mlx, prog->mlx_win);
		}
		if (prog->mlx)
		{
			mlx_destroy_display(prog->mlx);
			free(prog->mlx);
		}
	}
	// ! note: t_prog should lives in stack
	exit(1);
}
