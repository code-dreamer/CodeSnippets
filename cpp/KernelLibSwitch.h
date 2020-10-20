#pragma once

//--------------------------------------------------------------------
//
// It defines the way the library is built (static or dynamic)
//
// If you want to build TorrentDownloader library as a static
// or fully include it to your project, you should define KERNEL_LIB.
// Otherwise, define KERNEL_DLL.
//
//--------------------------------------------------------------------

#ifndef KERNEL_API
	#if defined(KERNEL_LIB)
		#define KERNEL_API		// build as a static library
	#else
		#ifdef KERNEL_DLL
			#define KERNEL_API		Q_DECL_EXPORT //__declspec(dllexport)
		#else
			#define KERNEL_API		Q_DECL_IMPORT //__declspec(dllimport)
		#endif
	#endif
#endif