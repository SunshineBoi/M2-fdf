/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_exit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:49:32 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 14:54:57 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

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

