//======================================================================
//
//        Copyright (C) 20017-2018 �����ϷУ��
//        All rights reserved
//
//        filename :
//        description :
//
//        created by ����
//	  QQ:251733538
//	  �ǳ�:С����
//        ����:251733538@qq.com
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
