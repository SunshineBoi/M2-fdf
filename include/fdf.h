/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:40 by kong              #+#    #+#             */
/*   Updated: 2026/04/30 19:00:00 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FDF_H
# define FDF_H

# include "mlx.h"
# include "get_next_line.h"
# include <X11/keysym.h>  // definition for different key pressed "keysymdef.h"
# include <X11/Xlib.h>
# include "mlx_int.h"
# include <fcntl.h>
# include <stdlib.h>
# include <stdio.h>
# include <limits.h>
# include <math.h>

# define WIN_WIDTH 1500
# define WIN_HEIGHT 900
// # define COLOR_POSITIVE 0x800080
# define COLOR_POSITIVE 0x03AC13
# define COLOR_NEGATIVE 0xFF0000
# define COLOR_BASE 0xFFFFFF
# define DEG_TO_RAD (3.14159265358979323846 / 180.0)

typedef struct s_mlx
{
	void	*conn_ptr;
	void	*win_ptr;
	int		is_clickhold;
	int		mouse_x;
	int		mouse_y;
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

typedef struct	s_viewpoint
{
	int		win_width;
	int		win_height;
	int		x_win_anchor;
	int		y_win_anchor;
	float	f_zoom;
	float	f_zscale;
	double	spin_deg;
	double	pitch_deg;
}	t_viewpoint;

// ! check type
typedef struct	s_image
{
	int		is_dirty;
	int		bpp;
	int		spl;
	int		endian;
	void	*img_ptr;
	char	*pixel_ptr;
}	t_image;

typedef struct s_data
{
	struct s_mlx		*mlx_obj;
	struct s_map		*map_obj;
	struct s_viewpoint	*vp_obj;
	struct s_image		*img_obj;
}	t_data;

// utils_draw.c

int	rgb_to_int(int r, int g, int b);

// utils_exit.c

void	perrexit(char *msg, int code);
void	errexit(char *msg, int code);
void	freelst_perrexit(char **lst, char *msg, int code);
void	freelst_exit(char **lst, char *msg, int code);

// utils_file.c

int		open_file_read(char *file_path);

// utils_free.c

void	freelst(void **lst);
void	freemlx(t_mlx *mlx);
int		freeprogexit(t_data *prog, int code);

// utils_map.c

int ft_count_list(char **str);
int	count_word_by_delim(char *str, char ch);
char	**ft_split_by_delim(char *str, char delim);

// utils_math.c

int	ft_abs(int nbr);
int	ft_max(int a, int b);

// utils_mem.c

// void	*ft_memcpy(void *dest, const void *src, size_t n);
void *ft_memset(void *s, int value, size_t nbyte);
void	**ft_calloc_lst(size_t size);
void	**ft_realloc_lst(void **lst, size_t new_size);

// utils_print.c

void	ft_putstr_fd(char *str, int fd);
void	ft_putchar_fd(char ch, int fd);
void	print_errmsg(char *msg);

// utils_str.c

int	ft_iswhitespace(const char ch);
int	ft_atoi(const char *nptr);
int	ft_color_hexatoi(const char *nptr);
int	is_hex(char ch);

// utils_validate.c

int	is_valid_int(char *str);
int	is_valid_color(char *str);

// draw.c
int	rerender_img(t_data *data);
int	create_mlx_img(t_mlx *mlx_obj, t_image *img_obj, t_viewpoint *vp_obj);
t_image	*init_img(t_mlx *mlx_obj, t_viewpoint *vp_obj);
void	draw_line(t_data *data, t_coord start, t_coord end);
void	do_render(t_data *data, t_coord *screen_arr);
int	render_img(t_data *data);
int	interpolate_color_from_base(float z_curr, t_coord start, t_coord end);
int	interpolate_color(int color_start, int color_end, float f);

// events.c

int	loop_hook(void *param);
int	handle_close(void *param);
int	handle_keyboard(int keycode, void *param);
void	setup_events(t_data *data);
int	handle_resize(void *param);

// main

t_data	*init_data();
t_mlx	*setup_mlx();

// map

t_map	*init_map(void);
t_coord	*parse_coord_arr(char *str, t_map* map);
t_map	*build_map(int fd);

// viewpoint.c

t_viewpoint	*init_vp(t_map *map);
void	iso_vp(t_viewpoint *vp, int rows, int cols);
void	set_vp(t_data *data, double spin_deg, double pitch_deg);


#endif