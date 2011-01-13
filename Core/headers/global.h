/***********************************************************************************************************************
 * global.h
 *
 *  Created on: Jan 13, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef GLOBAL_H_
#define GLOBAL_H_

template <class T> inline void SAFE_DELETE( T* & object)
{
	if (object)
	{
		delete object;
		object = NULL;
	}
}

#endif /* GLOBAL_H_ */
