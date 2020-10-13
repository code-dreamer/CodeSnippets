#pragma once

#define NO_ASSIGN_CLASS(ClassName)					\
	private:										\
		ClassName &operator=(const ClassName &);	\
		ClassName &operator=(const ClassName &&);

#define NO_COPY_CLASS(ClassName)					\
	private:										\
		ClassName(const ClassName &);				\
		ClassName(const ClassName &&);				

#define NO_COPY_ASSIGN_CLASS(ClassName)				\
	private:										\
		ClassName(const ClassName &);				\
		ClassName(const ClassName &&);				\
		ClassName &operator=(const ClassName &);	\
		ClassName &operator=(const ClassName &&);

#define NOOP __noop

/* x=target variable, y=mask */
#define BIT_SET(x,y) ((x) |= (y))
#define BIT_CLEAR(x,y) ((x) &= (~(y)))
#define BIT_FLIP(x,y) ((x) ^= (y))
#define BIT_CHECK(x,y) ((x) & (y))

#define _UNUSED(identifier) /* identifier */

