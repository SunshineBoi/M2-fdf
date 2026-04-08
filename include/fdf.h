/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:40 by kong              #+#    #+#             */
/*   Updated: 2026/04/09 00:22:45 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FDF_H
# define FDF_H

# include "mlx.h"
# include <X11/keysym.h>  // definition for different key pressed "keysymdef.h"
# include "stdlib.h"

# define WIN_WIDTH 800
# define WIN_HEIGHT 600

typedef struct s_mlx
{
	void	*conn;
	void	*win;
}	t_mlx;

// struct for map
typedef struct	s_map
{
	int	width;
	int	height;
	
}

// struct for point (map can contains multiple point)

// struct for img

typedef struct s_prog
{
	struct s_mlx	*self_mlx;
}	t_prog;

#endif