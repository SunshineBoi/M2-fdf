/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_str.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 22:58:04 by kong              #+#    #+#             */
/*   Updated: 2026/04/25 23:33:28 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

int	ft_iswhitespace(const char ch)
{
	return (ch == ' ' || ch == '\n' || ch == '\t'
		|| ch == '\f' || ch == '\v' || ch == '\r');
}

int	ft_atoi(const char *nptr)
{
	int	sign;
	int	res;

	while (ft_iswhitespace(*nptr))
		nptr++;
	sign = 1;
	if (*nptr == '+' || *nptr == '-')
	{
		if (*nptr == '-')
			sign = -sign;
		nptr++;
	}
	res = 0;
	while (*nptr >= '0' && *nptr <= '9')
	{
		res = (res * 10) + (*nptr - '0');
		nptr++;
	}
	return (sign * res);
}

static int	_val(char ch)
{
	if (ch >= '0' && ch <= '9')
		return (ch - '0');
	if (ch >= 'a' && ch <= 'f')
		return (ch - 'a' + 10);
	if (ch >= 'A' && ch <= 'F')
		return (ch - 'A' + 10);
	return (-1);
}

int	ft_color_hexatoi(const char *nptr)
{
	int	res;
	int	nbr;

	// skip '0x'
	nptr += 2;
	res = 0;
	while (*nptr)
	{
		nbr = _val(*nptr);
		res = (res * 16) + nbr;
		nptr++;
	}
	return (res);
}

int	is_hex(char ch)
{
	return ((ch >= '0' && ch <= '9') ||
			(ch >= 'a' && ch <= 'f') ||
			(ch >= 'A' && ch <= 'F'));
}

size_t	ft_strlen(const char *str)
{
	size_t	count;

	if (!str)
		return (0);
	count = 0;
	while (str[count])
		count++;
	return (count);
}
