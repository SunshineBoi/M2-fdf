/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_exit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:49:32 by kong              #+#    #+#             */
/*   Updated: 2026/04/27 18:38:26 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"


// void	input_errexit(char **lst)
// {
// 	if (lst)
// 		free_list(lst);
// 	ft_putstr_fd("Error\n", 2);
// 	exit(1);
// }

// sys error exit
void	perrexit(char *msg, int code)
{
	perror(msg);
	exit(code);
}

void	errexit(char *msg, int code)
{
	if (msg)
	{
		ft_putstr_fd(msg, 2);
		ft_putchar_fd('\n', 2);
	}
	exit(code);
}

void	freelst_perrexit(char **lst, char *msg, int code)
{
	freelst((void **)lst);
	perrexit(msg, code);
}

void	freelst_exit(char **lst, char *msg, int code)
{
	freelst((void **)lst);
	errexit(msg, code);
}

