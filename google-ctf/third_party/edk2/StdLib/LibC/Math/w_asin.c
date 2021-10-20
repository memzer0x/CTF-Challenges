/* @(#)w_asin.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */
#include  <LibConfig.h>
#include  <sys/EfiCdefs.h>
#if defined(LIBM_SCCS) && !defined(lint)
__RCSID("$NetBSD: w_asin.c,v 1.9 2002/05/26 22:01:59 wiz Exp $");
#endif

/*
 * wrapper asin(x)
 */


#include "math.h"
#include "math_private.h"


double
asin(double x)    /* wrapper asin */
{
#ifdef _IEEE_LIBM
  return __ieee754_asin(x);
#else
  double z;
  z = __ieee754_asin(x);
  if(_LIB_VERSION == _IEEE_ || isnan(x)) return z;
  if(fabs(x)>1.0) {
          return __kernel_standard(x,x,2); /* asin(|x|>1) */
  } else
      return z;
#endif
}
