/*************************************************************************
	> File Name: DEBUG.h
	> Author: 
	> Mail: 
	> Created Time: 2019年02月28日 星期四 10时09分06秒
 ************************************************************************/

#ifndef _DEBUG_H

#ifdef _DEBUG
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define _DEBUG_H
#endif

