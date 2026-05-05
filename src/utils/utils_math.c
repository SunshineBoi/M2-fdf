/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_math.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 18:21:00 by kong              #+#    #+#             */
/*   Updated: 2026/05/05 16:40:00 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

long	ft_abs(int nbr)
{
	if (nbr < 0)
		return (-(long)nbr);
	return (nbr);
}

int	ft_max(int a, int b)
{
	if (a >= b)
		return (a);
	return (b);
}
