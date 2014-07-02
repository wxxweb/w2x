/******************************************************************************
文件:	w2x_common.h
描述:	本头文件包含了 w2x_common 模块中所有导出头文件，仅以方便为目的，供外部使用。但是，
		仍旧建议根据具体需求包含所对应的导出头文件，而非直接包含本头文件。直接包含可能会导致
		编译的目标文件（.obj文件）增大，从而在某种程度上影响到编译和链接速度。如果确实需要
		包含本头文件，建议在外部项目模块的预定义头文件中包含，这样将不会影响编译和链接速度。
		另外，严禁在整个 w2x 项目内部包含该头文件。
作者:	wu.xiongxing
时间:	2013-05-24
修改:	2014-07-02
*******************************************************************************/

#ifndef __W2X_COMMON_COMMON_H__
#define __W2X_COMMON_COMMON_H__

#ifndef W2X_COMMON_EXPORTS ///< 禁止在 w2x_common 模块内部包含本头文件

#include "./base.h"
#include "./command.h"
#include "./debug.h"
#include "./encode.h"
#include "./error.h"
#include "./event.h"
#include "./event_dispatcher.h"
#include "./file_sys.h"
#include "./macros.h"
#include "./msg_loop.h"
#include "./mutex.h"
#include "./process.h"
#include "./ref_ptr.h"
#include "./sys_tray.h"
#include "./system.h"
#include "./thread.h"
#include "./variable.h"
#include "./version.h"

#endif ///< !W2X_COMMON_EXPORTS


#endif ///< !__W2X_COMMON_COMMON_H__