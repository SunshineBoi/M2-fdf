/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdf.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 14:23:40 by kong              #+#    #+#             */
/*   Updated: 2026/05/06 19:27:02 by kong             ###   ########.fr       */
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

typedef struct s_line
{
	t_coord	start;
	t_coord	end;
	int		major_axis;
	int		minor_axis;
	int		major_step;
	int		minor_step;
	int		gradient;
	int		dx;
	int		dy;
	int		boundary;
	int		reset;
}	t_line;

// (OK) utils_draw.c

int		rgb_to_int(int r, int g, int b);
int		interpolate_color(int color_start, int color_end, float f);
int		interpolate_color_from_base(float z_curr, t_coord start, t_coord end);
void	color_pixel(t_data *data, int x, int y, int color);

// (OK) utils_exit.c

void	perrexit(char *msg, int code);
void	errexit(char *msg, int code);
void	freelst_perrexit(char **lst, char *msg, int code);
void	freelst_exit(char **lst, char *msg, int code);

// (OK) utils_free.c

void	freelst(void **lst);
void	freemlx(t_mlx *mlx);
int		freeprogexit(t_data *prog, int code);

// (OK) utils_image.c
void	build_img_points(t_data *data, t_coord *points, int rows, int cols);

// (OK) utils_map.c

char	*ft_strnstr(const char *big, const char *little, size_t len);
int 	ft_count_list(char **str);
int		count_word_by_delim(char *str, char ch);
char	**ft_split_by_delim(char *str, char delim);

// (OK) utils_math.c

long	ft_abs(int nbr);
int		ft_max(int a, int b);
float	ft_min(float a, float b);

// (OK) utils_mem.c

void 	*ft_memset(void *s, int value, size_t nbyte);
void	**ft_calloc_lst(size_t size);
void	**ft_realloc_lst(void **lst, size_t new_size);

// (OK) utils_mouse.c

void	mouse_zoom(t_data *data, int x, int y, float f_zoom);
void	set_pan(t_data *data, int x, int y);
void	set_rotate(t_data *data, int x, int y);
void	mouse_pan(t_data *data, int x, int y);
void	mouse_rotate(t_data *data, int x, int y);

// (OK) utils_print.c

void	ft_putstr_fd(char *str, int fd);
void	ft_putchar_fd(char ch, int fd);
void	perrmsg(char *msg);

// (OK) utils_str.c

int		ft_iswhitespace(const char ch);
int		ft_atoi(const char *nptr);
int		ft_color_hexatoi(const char *nptr);
int		is_hex(char ch);

// (OK) utils_validate.c

int		is_valid_int(char *str);
int		is_valid_color(char *str);

// (OK) draw.c

t_line	init_line(t_coord start, t_coord end);
void	draw_line(t_data *data, t_coord start, t_coord end);

// (OK) events_key.c

int		handle_keyboard(int keycode, void *param);
int		handle_mouse_click(int button, int x, int y, void *param);
int		handle_mouse_release(int button, int x, int y, void *param);
int		handle_mouse_move(int x, int y, void *param);

// (OK) events.c

int		handle_close(void *param);
int		handle_resize(void *param);
int		handle_loop_hook(void *param);


// (OK) image.c

int		init_mlx_img(t_mlx *mlx_obj, t_image *img_obj, t_viewpoint *vp_obj);
t_image	*init_img(t_mlx *mlx_obj, t_viewpoint *vp_obj);
int		rerender_img(t_data *data);
int		render_img(t_data *data);

// main.c

t_data	*init_data(void);
t_mlx	*setup_mlx(void);
void	setup_events(t_data *data);
int		fdf(char *file);

// (OK) map_points.c

t_coord	*parse_coord_arr(char *str, t_map* map);

// (OK) map

t_map	*init_map(void);
t_map	*build_map(int fd);
t_map	*read_map_file(char *file_path);

// (OK) viewpoint.c

t_viewpoint	*init_vp(t_map *map);
void	iso_vp(t_viewpoint *vp, int rows, int cols);
void	set_vp(t_data *data, double spin_deg, double pitch_deg);


#endif
