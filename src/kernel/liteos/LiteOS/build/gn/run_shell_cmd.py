#!/usr/bin/env python3
# coding=utf-8
# Copyright © Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
import os
import sys


def main():
    """Function run the compilation command."""
    return os.WEXITSTATUS(os.system(' '.join(sys.argv[1:])))

if __name__ == '__main__':
    sys.exit(main())
