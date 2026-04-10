/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_print.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/09 17:08:47 by kong              #+#    #+#             */
/*   Updated: 2026/04/09 17:09:00 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void	ft_putstr_fd(char *str, int fd)
{
	if (!str)
		return ;
	while (*str)
		write(fd, str++, 1);
}

void	ft_putchar_fd(char ch, int fd)
{
	write(fd, &ch, 1);
}
