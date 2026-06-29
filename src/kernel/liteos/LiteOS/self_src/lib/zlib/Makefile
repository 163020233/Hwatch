include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := z

LOCAL_SRCS := adler32.c compress.c crc32.c deflate.c gzclose.c gzlib.c \
			  gzread.c gzwrite.c infback.c inffast.c inflate.c inftrees.c \
			  trees.c uncompr.c zutil.c
LITEOS_CFLAGS += -DZ_HAVE_UNISTD_H
LOCAL_SRCS := $(addprefix $(LITEOS_OPENSOURCE_PATHNAME)/zlib/, $(LOCAL_SRCS))
LOCAL_SRCS_NEED_SORT := y

include $(MODULE)
