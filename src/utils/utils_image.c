/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kong <kong@student.42singapore.sg>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 18:03:02 by kong              #+#    #+#             */
/*   Updated: 2026/04/07 18:03:15 by kong             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

// in 1-byte: r = 0xFF (hex) / 255 (dec)
// int has 4-bytes, doing bitshift to form the full integer.
int	rgb_to_int(int r, int g, int b)
{
	return (r << 16 | g << 8 | b);
}
