//======================================================================
//
//        Copyright (C) 20017-2018 虹口游戏校区
//        All rights reserved
//
//        filename :
//        description :
//
//        created by 胡凯
//	  QQ:251733538
//	  昵称:小飞侠
//        邮箱:251733538@qq.com
//
//======================================================================


/** @file assert.h
 */
#ifndef AI_DEBUG_H_INC
#define AI_DEBUG_H_INC

#ifdef ASSIMP_BUILD_DEBUG  
#	include <assert.h>
#	define	ai_assert(expression) assert(expression)
#else
#	define	ai_assert(expression)
#endif


#endif
