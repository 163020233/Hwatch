/****************************************************************************
 * fs/vfs/fs_read.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <assert.h>
#include <errno.h>

#include <nuttx/cancelpt.h>
#ifdef CONFIG_VFS_READ_EXTEND
#include "nuttx/fs/fs.h"
#endif
#include "notify/notify.h"
#include "inode/inode.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: file_readv_compat
 *
 * Description:
 *   Emulate readv using file_operation::read.
 *
 *   Unless iovcnt <= 1, this implementation is NOT appropriate for files
 *   with non-trivial semantics, including:
 *
 *     - Files which might return partial success. (except the EOF)
 *       (Eg. certain character devices, including tty.)
 *
 *     - Files which need to preserve data boundaries.
 *       (Eg. datagram sockets)
 *
 *     - Files which need to provide read/write atomicity.
 *       (Eg. regular files, pipes, fifos. Note that, although NuttX
 *       doesn't implement the atomicity for regular files as of writing
 *       this, POSIX requires it.)
 *
 *     - Files with flow-control mechanisms might be confused a bit by
 *       this implementation. (Eg. TCP socket)
 *
 *   For those kind of files, please consider to implement
 *   file_operations::readv natively instead of using this function.
 *
 ****************************************************************************/

static ssize_t file_readv_compat(FAR struct file *filep,
                                 FAR const struct iovec *iov, int iovcnt)
{
  FAR struct inode *inode = filep->f_inode;
  ssize_t ntotal;
  ssize_t nread;
  int i;

  /* Process each entry in the struct iovec array */

  for (i = 0, ntotal = 0; i < iovcnt; i++)
    {
      /* Ignore zero-length reads */

      if (iov[i].iov_len == 0)
        {
          continue;
        }

      /* Sanity check to avoid total length overflow */

      if (SIZE_MAX - ntotal < iov[i].iov_len)
        {
          if (ntotal > 0)
            {
              break;
            }

          return -EINVAL;
        }

      nread = inode->u.i_ops->read(filep, iov[i].iov_base,
                                   iov[i].iov_len);

      /* Check for a read error */

      if (nread < 0)
        {
          if (ntotal > 0)
            {
              break;
            }

          return nread;
        }

      ntotal += nread;

      /* Check for a partial success condition, including an end-of-file */

      if (nread < (ssize_t)iov[i].iov_len)
        {
          break;
        }
    }

  return ntotal;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: file_readv
 *
 * Description:
 *   file_readv() is an internal OS interface.  It is functionally similar to
 *   the standard readv() interface except:
 *
 *    - It does not modify the errno variable,
 *    - It is not a cancellation point,
 *    - It accepts a file structure instance instead of file descriptor.
 *
 * Input Parameters:
 *   filep  - File structure instance
 *   iov    - User-provided iovec to save the data
 *   iovcnt - The number of iovec
 *
 * Returned Value:
 *   The positive non-zero number of bytes read on success, 0 on if an
 *   end-of-file condition, or a negated errno value on any failure.
 *
 ****************************************************************************/

ssize_t file_readv(FAR struct file *filep,
                   FAR const struct iovec *iov, int iovcnt)
{
  FAR struct inode *inode;
  ssize_t ret;

  DEBUGASSERT(filep);
  inode = filep->f_inode;

  /* Check buffer count and pointer for iovec */

  if (iovcnt == 0)
    {
      return 0;
    }

  if (iov == NULL)
    {
      return -EFAULT;
    }

  /* Are all iov_base accessible? */

  for (ret = 0; ret < iovcnt; ret++)
    {
      if (iov[ret].iov_base == NULL && iov[ret].iov_len != 0)
        {
          return -EFAULT;
        }
    }

  ret = -EBADF;

  /* Was this file opened for read access? */

#ifdef CONFIG_VFS_POSIX_OFLAGS
  if (vfs_posix_oflags_is_writeonly(filep->f_oflags))
#else
  if ((filep->f_oflags & O_RDOK) == 0)
#endif
    {
      /* No.. File is not read-able */

      ret = -EACCES;
    }

  /* Is a driver or mountpoint registered? If so, does it support the read
   * method?
   * If yes, then let it perform the read.  NOTE that for the case of the
   * mountpoint, we depend on the read methods being identical in
   * signature and position in the operations vtable.
   */

  else if (inode != NULL && inode->u.i_ops)
    {
#ifndef CONFIG_VFS_DISABLE_VECTORED_IO
      if (inode->u.i_ops->readv)
        {
          struct uio uio;

          ret = uio_init(&uio, iov, iovcnt);
          if (ret == 0)
            {
              ret = inode->u.i_ops->readv(filep, &uio);
            }
	  
        }
      else if (inode->u.i_ops->read)
#else
      if (inode->u.i_ops->read)
#endif      
        {
          ret = file_readv_compat(filep, iov, iovcnt);
        }
    }

  /* Return the number of bytes read (or possibly an error code) */

#ifdef CONFIG_FS_NOTIFY
  if (ret > 0)
    {
      notify_read(filep);
    }
#endif

  return ret;
}

/****************************************************************************
 * Name: file_read
 *
 * Description:
 *   file_read() is an internal OS interface.  It is functionally similar to
 *   the standard read() interface except:
 *
 *    - It does not modify the errno variable,
 *    - It is not a cancellation point,
 *    - It accepts a file structure instance instead of file descriptor.
 *
 * Input Parameters:
 *   filep  - File structure instance
 *   buf    - User-provided to save the data
 *   nbytes - The maximum size of the user-provided buffer
 *
 * Returned Value:
 *   The positive non-zero number of bytes read on success, 0 on if an
 *   end-of-file condition, or a negated errno value on any failure.
 *
 ****************************************************************************/

ssize_t file_read(FAR struct file *filep, FAR void *buf, size_t nbytes)
{
  struct iovec iov;

  if (buf == NULL)
    {
      return -EFAULT;
    }

  iov.iov_base = buf;
  iov.iov_len = nbytes;

  return file_readv(filep, &iov, 1);
}

/****************************************************************************
 * Name: nx_readv
 *
 * Description:
 *   nx_readv() is an internal OS interface.  It is functionally similar to
 *   the standard readv() interface except:
 *
 *    - It does not modify the errno variable, and
 *    - It is not a cancellation point.
 *
 * Input Parameters:
 *   fd     - File descriptor to read from
 *   iov    - User-provided iovec to save the data
 *   iovcnt - The number of iovec
 *
 * Returned Value:
 *   The positive non-zero number of bytes read on success, 0 on if an
 *   end-of-file condition, or a negated errno value on any failure.
 *
 ****************************************************************************/

ssize_t nx_readv(int fd, FAR const struct iovec *iov, int iovcnt)
{
  FAR struct file *filep;
  ssize_t ret;

  /* First, get the file structure.  Note that on failure,
   * fs_getfilep() will return the errno.
   */

  ret = (ssize_t)fs_getfilep(fd, &filep);
  if (ret >= 0)
    {
      /* Then let file_readv do all of the work. */

      ret = file_readv(filep, iov, iovcnt);

      fs_putfilep(filep);
    }

  return ret;
}

/****************************************************************************
 * Name: nx_read
 *
 * Description:
 *   nx_read() is an internal OS interface.  It is functionally similar to
 *   the standard read() interface except:
 *
 *    - It does not modify the errno variable, and
 *    - It is not a cancellation point.
 *
 * Input Parameters:
 *   fd     - File descriptor to read from
 *   buf    - User-provided to save the data
 *   nbytes - The maximum size of the user-provided buffer
 *
 * Returned Value:
 *   The positive non-zero number of bytes read on success, 0 on if an
 *   end-of-file condition, or a negated errno value on any failure.
 *
 ****************************************************************************/

ssize_t nx_read(int fd, FAR void *buf, size_t nbytes)
{
  struct iovec iov;

  iov.iov_base = buf;
  iov.iov_len = nbytes;
  return nx_readv(fd, &iov, 1);
}

/****************************************************************************
 * Name: readv
 *
 * Description:
 *   The standard, POSIX read interface.
 *
 * Input Parameters:
 *   fd     - File descriptor to read from
 *   iov    - User-provided iovec to save the data
 *   iovcnt - The number of iovec
 *
 * Returned Value:
 *   The positive non-zero number of bytes read on success, 0 on if an
 *   end-of-file condition, or -1 on failure with errno set appropriately.
 *
 ****************************************************************************/

ssize_t readv(int fd, FAR const struct iovec *iov, int iovcnt)
{
  ssize_t ret;

#ifdef CONFIG_VFS_READV_VERIFY
  if (iov == NULL || iovcnt <= 0)
    {
      set_errno(EINVAL);
      return -1;
    }
#endif
  /* readv() is a cancellation point */

  enter_cancellation_point();

#ifdef CONFIG_VFS_SOCKET
  if (vfs_socket_fd_verify(fd) && iov != NULL)
    {
      ret = vfs_read_socket(fd, iov->iov_base, iov->iov_len);
      leave_cancellation_point();
      return ret;
    }
#endif

#ifdef CONFIG_VFS_CONSOLE
  if (vfs_console_fd_verify(fd) && vfs_console_update_fd(&fd) != OK)
    {
      set_errno(EBADF);
      leave_cancellation_point();
      return ERROR;
    }
#endif

  /* Let nx_readv() do the real work */

  ret = nx_readv(fd, iov, iovcnt);
  if (ret < 0)
    {
      set_errno(-ret);
      ret = ERROR;
    }

  leave_cancellation_point();
  return ret;
}

/****************************************************************************
 * Name: read
 *
 * Description:
 *   The standard, POSIX read interface.
 *
 * Input Parameters:
 *   fd     - File descriptor to read from
 *   buf    - User-provided to save the data
 *   nbytes - The maximum size of the user-provided buffer
 *
 * Returned Value:
 *   The positive non-zero number of bytes read on success, 0 on if an
 *   end-of-file condition, or -1 on failure with errno set appropriately.
 *
 ****************************************************************************/

#ifdef CONFIG_VFS_USE_POSIX_ALIAS_API 
ssize_t read_alias(int fd, FAR void *buf, size_t nbytes)
#else
ssize_t read(int fd, FAR void *buf, size_t nbytes)
#endif
{
  struct iovec iov;

  iov.iov_base = buf;
  iov.iov_len = nbytes;
  return readv(fd, &iov, 1);
}
