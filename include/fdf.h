/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:40 by kong              #+#    #+#             */
/*   Updated: 2026/04/18 21:19:33 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FDF_H
# define FDF_H

# include "mlx.h"
# include "get_next_line.h"
# include <X11/keysym.h>  // definition for different key pressed "keysymdef.h"
# include <fcntl.h>
# include <stdlib.h>
# include <stdio.h>
# include <limits.h>

# define WIN_WIDTH 800
# define WIN_HEIGHT 600

typedef struct s_mlx
{
	void	*conn_ptr;
	void	*win_ptr;
}	t_mlx;

typedef struct	s_map
{
	int	n_col;
	int	n_row;
	int	z_min;
	int	z_max;
	struct s_coord	**coords_lst;
}	t_map;

typedef struct	s_coord
{
	int	z_height;
	int	color;
}	t_coord;

typedef struct	s_viewpoint
{
	int	zoom;
	int	x_off;
	int	y_off;
	int	z_scale;
}	t_viewpoint;

typedef struct	s_image
{
	int	bpp;
	int	llen;
	int	endian;
	int	*img_ptr;
}	t_image;

typedef struct s_prog
{
	struct s_mlx	*mlx_obj;
	struct s_map	*map_obj;
	struct s_viewpoint	*vp_obj;
	
}	t_prog;

// utils_exit.c
void	perror_exit(char *msg, int code);
void	errexit(char *msg, int code);
void	free_lst_and_exit(char **lst, char *msg, int code);

// utils_file.c
int	open_file_as_read(char *file_path);

// utils_free.c
void	destroy_mlx(t_mlx *mlx);
void	free_lst(char **lst);
int	free_prog(t_prog *prog, int code);

// utils_image.c
int	rgb_to_int(int r, int g, int b);

// utils_init.c
t_map	*init_map(void);

// utils_mem.c
void	**ft_calloc_lst(size_t size);
void	**ft_realloc_lst(void **lst, size_t new_size);

// utils_parse.c
int ft_count_list(char **str);
int	count_word_by_delim(char *str, char ch)
char	**ft_split_by_delim(char *str, char delim);

// utils_print.c
void	ft_putstr_fd(char *str, int fd);
void	ft_putchar_fd(char ch, int fd);
void	print_error_msg(char *msg);

// utils_str.c
int	ft_iswhitespace(const char ch);
int	ft_atoi(const char *nptr);
unsigned int	ft_color_hexatoi(const char *nptr);
int	is_hex(char ch);
size_t	ft_strlen(const char *str);

// utils_validate.c
int	is_valid_int(char *str);
int	is_valid_color(char *str);

#endif