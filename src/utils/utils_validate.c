/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_validate.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/12 13:50:58 by kong              #+#    #+#             */
/*   Updated: 2026/04/13 17:25:56 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

static int	_is_in_range(char *str, long long limit)
{
	long long	total;
	int			digit;

	total = 0;
	while (*str)
	{
		if (!(*str >= '0' && *str <= '9'))
			return (0);
		digit = *str - '0';
		if (total > (limit / 10) || (total == (limit / 10) && digit > (limit % 10)))
			return (0);	
		total = (total * 10) + digit;
		str++;
	}
	return (1);
}

int	is_valid_int(char *str)
{
	long long	limit;

	if (!str || !*str)
		return (0);
	limit = INT_MAX;
	// check sign
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			limit = -INT_MIN;
		str++;
	}
	// check if sign only
	if (!*str)
		return (0);
	return (_is_in_range(str, limit));
}

int	is_valid_color(char *str)
{
	// prefix with 0x
	// proper hex based
	// no overflow
	// no null
	int	limit;

	if (!str || !*str)
		return (0);
	if (*str != '0' || *(str + 1) != 'x')
		return (0);
	str += 2;
	if (!str)
		return (0);
	limit = 8;
	while (*str && limit--)
	{
		if (!is_hex(*str))
			return (0);
		str++;
	}
	if (*str != '\0')
		return (0);
	return (1);		
}
