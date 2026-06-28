# HiSpark API 信息汇总


## 概述

本文档提供HiSpark当前API信息统计。
信息含：主要模块，及子模块，其API对外头文件路径，API函数数量等信息。
为工程管控，及AI识别分析提供基线信息。


## 对外API信息汇总表 

### 内核、驱动类接口
* 模块：driver
  * 模块函数数量：807
  * 模块路径: /include/driver

* 模块：kernel
  * 模块函数数量：1492
  * 模块路径：见各子模块路径
  * 子模块信息：
    1. cmsis v1.0
       * 子模块函数数量：45
       * 子模块路径：kernel/liteos/LiteOS/Huawei_LiteOS/self_src/compat/cmsis/cmsis_os1.h
    2. cmsis v2.0
       * 子模块函数数量：71
       * 子模块路径：kernel/liteos/LiteOS/Huawei_LiteOS/open_source/CMSIS_6/CMSIS/RTOS2/Include/cmsis_os2.h
    3. posix
       * 子模块函数数量：231
       * 子模块路径：kernel/liteos/LiteOS/Huawei_LiteOS/open_source/musl/include
    4. rtt
       * 子模块函数数量：434
       * 子模块路径：kernel/liteos/LiteOS/Huawei_LiteOS/self_src/compat/rt-thread/src
    5. freertos
       * 子模块函数数量：175
       * 子模块路径：kernel/liteos/LiteOS/Huawei_LiteOS/self_src/compat/freertos/src
    6. osal
       * 子模块函数数量：264
       * 子模块路径：kernel/osal/include
    7. LOS
       * 子模块函数数量：272
       * 子模块路径：kernel/liteos/LiteOS/Huawei_LiteOS/self_src/kernel_lite/include

### 系统、业务类接口

* 模块：ai (驱动侧、turing侧接口)
  * 模块函数数量：81
  * 模块路径: /include/middleware/services/ai

* 模块：ai (基于turing接口的封装)
  * 模块函数数量：19
  * 模块路径: /include/middleware/utils/ai.h

* 模块：bts (L2)
  * 模块函数数量：376
  * 模块路径: /include/middleware/services/bts

* 模块：bts (L3)
  * 模块函数数量：167
  * 模块路径: /protocol/bt/host/include


* 模块：gnss
  * 模块函数数量：18
  * 模块路径: /include/middleware/services/gnss

* 模块：media
  * 模块函数数量：192
  * 模块路径: 见各子模块路径
  * 子模块信息：
    1. AudioPlayer
       * 子模块函数数量：29
       * 子模块路径：middleware/services/media/foundation/player/interfaces/kits/
    2. AudioManager
       * 子模块函数数量：24
       * 子模块路径：middleware/services/media/foundation/audiomanager/interfaces/kits/audiomanager/cpp/
    3. AudioStreamIn
       * 子模块函数数量：33
       * 子模块路径：middleware/services/media/foundation/audiomanager/frameworks/audiostream/include/cpp
    4. AudioStreamOut
       * 子模块函数数量：11
       * 子模块路径：middleware/services/media/foundation/audio_capturer/interfaces/kits/
    5. AudioManagerA2dpSink
       * 子模块函数数量：8
       * 子模块路径：middleware/services/media/foundation/service/a2dp_sink_service/include
    6. StreamSourcePlayerService
       * 子模块函数数量：16
       * 子模块路径：middleware/services/media/foundation/service/stream_source_player_service/include
    7. VideoPlayerService
       * 子模块函数数量：18
       * 子模块路径：middleware/services/media/foundation/service/video_player_service/include
    8. AudioPlayerService
       * 子模块函数数量：28
       * 子模块路径：middleware/services/media/foundation/service/audio_player_service/include
    9. AudioStream或AudioManager
       * 子模块函数数量：25
       * 子模块路径：middleware/services/media/foundation/service/audio_manger_service/include

* 模块：graphic
  * 模块函数数量：611
  * 模块路径: 见各子模块路径
  * 子模块信息：
    1. common
       * 子模块函数数量：55
       * 子模块路径：middleware/services/gui/uikit/proprietary/include/common/
    2. components UI控件类
       * 子模块函数数量：418
       * 子模块路径：middleware/services/gui/uikit/proprietary/include/components
    3. dfx
       * 子模块函数数量：21
       * 子模块路径：middleware/services/gui/uikit/proprietary/include/dfx
    4. service
       * 子模块函数数量：24
       * 子模块路径：middleware/services/gui/uikit/proprietary/include/service
    5. lottie
       * 子模块函数数量：77
       * 子模块路径：middleware/services/gui/uikit/proprietary/include/lottie
    6. font
       * 子模块函数数量：9
       * 子模块路径：middleware/services/gui/uikit/proprietary/include/font
    7. gfx_utils
       * 子模块函数数量：7
       * 子模块路径：middleware/services/gui/uikit/proprietary/include/gfx_utils

* 模块：nfc
  * 模块函数数量：93
  * 模块路径: /include/middleware/services/nfc/controller

* 模块：at
  * 模块函数数量：8
  * 模块路径: /include/middleware/utils/at.h

* 模块：crc
  * 模块函数数量：3
  * 模块路径: /include/middleware/utils/uapi_crc.h

* 模块：diag
  * 模块函数数量：13
  * 模块路径: 
    /include/middleware/utils/diag.h
    /include/middleware/utils/hcd.h

* 模块：nv
  * 模块函数数量：13
  * 模块路径: /include/middleware/utils/nv.h

* 模块：partition
  * 模块函数数量：2
  * 模块路径: /include/middleware/utils/partition.h

* 模块：transmit
  * 模块函数数量：9
  * 模块路径: /include/middleware/utils/transmit.h

* 模块：upgrade
  * 模块函数数量：16
  * 模块路径: /include/middleware/utils/upg.h

* 模块：usb_class
  * 模块函数数量：141
  * 模块路径: /include/middleware/utils/usb_class

* 模块：psa
  * 模块函数数量：55
  * 模块路径: /include/middleware/utils/psa

* 模块：ipc_fs
  * 模块函数数量：2
  * 模块路径: /include/middleware/utils/ipc_fs

