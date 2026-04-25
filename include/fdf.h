/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:40 by kong              #+#    #+#             */
/*   Updated: 2026/04/24 10:38:25 by kong             ###   ########.fr       */
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
# include <math.h>

# define WIN_WIDTH 800
# define WIN_HEIGHT 600
# define DEG_TO_RAD (3.14159265358979323846 / 180.0)

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
	struct s_coord	**src_lst;
}	t_map;

typedef struct	s_coord
{
	int	x;
	int	y;
	int	z;
	int	color;
}	t_coord;

//! check type
typedef struct	s_viewpoint
{
	int	f_zoom;
	int	x_win_anchor;
	int	y_win_anchor;
	int	f_zscale;
	double spin_deg;
	double pitch_deg;
}	t_viewpoint;

// ! check type
typedef struct	s_image
{
	int	bpp;
	int	spl;
	int	endian;
	char	*pixel_ptr;
}	t_image;


// ! need update
typedef struct s_data
{
	struct s_mlx	*mlx_obj;
	struct s_map	*map_obj;
	struct s_viewpoint	*vp_obj;
	struct s_image	*img_obj;
}	t_data;

// utils_exit.c

void	perrexit(char *msg, int code);
void	errexit(char *msg, int code);
void	freelst_perrexit(char **lst, char *msg, int code);
void	freelst_exit(char **lst, char *msg, int code);

// utils_file.c

int		open_file_read(char *file_path);

// utils_free.c

void	freelst(char **lst);
void	freemlx(t_mlx *mlx);
int		freeprog(t_prog *prog, int code);

// utils_image.c

int	rgb_to_int(int r, int g, int b);

// utils_map.c

t_map	*init_map(void);
t_coord	*parse_coord_arr(char *str, t_map* map);

// utils_mem.c
// void	*ft_memcpy(void *dest, const void *src, size_t n);

void	**ft_calloc_lst(size_t size);
void	**ft_realloc_lst(void **lst, size_t new_size);

// utils_parse.c

int ft_count_list(char **str);
int	count_word_by_delim(char *str, char ch);
char	**ft_split_by_delim(char *str, char delim);

// utils_print.c

void	ft_putstr_fd(char *str, int fd);
void	ft_putchar_fd(char ch, int fd);
void	print_errmsg(char *msg);

// utils_str.c

int	ft_iswhitespace(const char ch);
int	ft_atoi(const char *nptr);
unsigned int	ft_color_hexatoi(const char *nptr);
int	is_hex(char ch);
size_t	ft_strlen(const char *str);

// utils_validate.c

int	is_valid_int(char *str);
int	is_valid_color(char *str);

// main


// map


// temp

t_viewpoint	*init_vp(t_map *map);

t_map	*build_map(int fd);

#endif