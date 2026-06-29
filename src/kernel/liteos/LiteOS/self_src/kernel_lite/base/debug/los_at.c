/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description : LiteOS shell commond adapt product AT commond.
 * Author : Huawei LiteOS Team
 * Create : 2025-9-11
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ctype.h"
#include "los_at.h"

/* Universal Command Callback Signature (Compatible with Existing Two Functions) */
typedef INT32 (*cmd_func_t)(INT32 argc, const CHAR **argv);

/* All support cmd types */
typedef enum {
    CMD_TYPE_SHOW = 0,
    CMD_TYPE_STD = 1,   // The cmdline parameter contains a command name.
    CMD_TYPE_EX = 2,    // The cmdline parameter does not contain command name.
    CMD_TYPE_BUTT
} CmdType;

typedef struct {
    const CHAR   *name;
    UINT32        min_argc;
    UINT32        max_argc;
    cmd_func_t    func;
    CmdType       cmdType;
    const CHAR   *help;
} LosCmdEntry;

typedef enum {
    TOKENIZE_OK = 0,
    TOKENIZE_ERR_INVALID_ARGS = -1,
    TOKENIZE_ERR_INPUT_TOO_LONG = -2,
    TOKENIZE_ERR_TOO_MANY_ARGS = -3,
} TokenizeResult;


/* Encapsulation to match the universal signature */
STATIC INT32 OsCmdSystemInfo(INT32 argc, const CHAR **argv)
{
    return OsShellCmdSystemInfo(argc, argv);
}

STATIC INT32 OsCmdTaskMem(INT32 argc, const CHAR **argv)
{
    return (INT32)OsDbgTskHeapInfoPrint((UINT32)argc, argv);
}

STATIC INT32 OsCmdTask(INT32 argc, const CHAR **argv)
{
    return (INT32)OsShellCmdDumpTask(argc, argv);
}

#ifdef LOSCFG_DEBUG_SWTMR
STATIC INT32 OsCmdSwtmr(INT32 argc, const CHAR **argv)
{
    return (INT32)OsShellCmdSwtmrInfoGet(argc, (const UINT8 **)argv);
}
#endif

#ifdef LOSCFG_DEBUG_SEMAPHORE
STATIC INT32 OsCmdSem(INT32 argc, const CHAR **argv)
{
    return (INT32)OsShellCmdMuxInfoGet(argc, argv);
}
#endif

#ifdef LOSCFG_DEBUG_MUTEX
STATIC INT32 OsCmdMutex(INT32 argc, const CHAR **argv)
{
    return (INT32)OsShellCmdMuxInfoGet(argc, argv);
}
#endif

#ifdef LOSCFG_DEBUG_QUEUE
STATIC INT32 OsCmdQueue(INT32 argc, const CHAR **argv)
{
    return (INT32)OsShellCmdQueueInfoGet(argc, argv);
}
#endif

#ifdef LOSCFG_DEBUG_MEM_MWMD
STATIC INT32 OsCmdMd(INT32 argc, const CHAR **argv)
{
    return (INT32)SystemMd(argc, argv);
}

STATIC INT32 OsCmdMw(INT32 argc, const CHAR **argv)
{
    return (INT32)SystemMw(argc, argv);
}
#endif

#ifdef LOSCFG_MEM_LEAKCHECK
STATIC INT32 OsCmdMemUsed(INT32 argc, const CHAR **argv)
{
    return (INT32)OsShellCmdMemUsed(argc, argv);
}
#endif
STATIC INT32 OsCmdFree(INT32 argc, const CHAR **argv)
{
    return (INT32)OsShellCmdFree(argc, argv);
}
STATIC INT32 OsCmdMemCheck(INT32 argc, const CHAR **argv)
{
    return (INT32)OsShellCmdMemCheck(argc, argv);
}
#if (defined LOSCFG_ARCH_INTERRUPT_TAKEOVER) && (defined LOSCFG_DEBUG_HWI)
STATIC INT32 OsCmdHwi(INT32 argc, const CHAR **argv)
{
    return (INT32)OsShellCmdHwi(argc, argv);
}
#endif
#ifdef LOSCFG_TRIGGER_ENABLE
STATIC INT32 OsCmdTrigger(INT32 argc, const CHAR **argv)
{
    return (INT32)OsTriggerProc(argc, argv);
}
#endif
#ifdef LOSCFG_KERNEL_CPUP
STATIC INT32 OsCmdCpup(INT32 argc, const CHAR **argv)
{
    return (INT32)OsShellCmdCpup(argc, argv);
}
#endif

#ifdef LOSCFG_KERNEL_PERF
STATIC INT32 OsCmdPerfStart(INT32 argc, const CHAR **argv)
{
    (VOID)argc;
    (VOID)argv;
    OsPerfHwPmuStart();
    return 0;
}
STATIC INT32 OsCmdPerfStop(INT32 argc, const CHAR **argv)
{
    (VOID)argc;
    (VOID)argv;
    LOS_PerfStop();
    return 0;
}
STATIC INT32 OsCmdPerfDump(INT32 argc, const CHAR **argv)
{
    (VOID)argc;
    (VOID)argv;
    OsPerfDataDump();
    return 0;
}
#endif

#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
STATIC INT32 OsCmdSchedStart(INT32 argc, const CHAR **argv)
{
    (VOID)argc;
    (VOID)argv;
    OsSchedStatStart();
    return 0;
}
STATIC INT32 OsCmdSchedStop(INT32 argc, const CHAR **argv)
{
    (VOID)argc;
    (VOID)argv;
    OsSchedStatStop();
    return 0;
}
STATIC INT32 OsCmdSchedInfo(INT32 argc, const CHAR **argv)
{
    (VOID)argc;
    (VOID)argv;
    OsSchedStatInfo();
    return 0;
}
#endif

#ifdef LOSCFG_MEMFRAG_DFX_SHOW_INFO
STATIC INT32 OsCmdMemFragInfo(INT32 argc, const CHAR **argv)
{
    (VOID)argc;
    (VOID)argv;
    LOS_MemFragInfo();
    return 0;
}
#endif

STATIC const LosCmdEntry g_osCmdTable[] = {
    { "systeminfo", 0, 0, OsCmdSystemInfo, CMD_TYPE_EX, "Show system resource usage. No args." },
    { "task_mem",   0, 2, OsCmdTaskMem,    CMD_TYPE_EX, "Show task heap info. Usage: task_mem [taskId]" },
    { "task",       0, 1, OsCmdTask,       CMD_TYPE_EX, "Show task info. No args." },
#ifdef LOSCFG_DEBUG_SWTMR
    { "swtmr",      0, 0, OsCmdSwtmr,      CMD_TYPE_EX, "Show swtmr info. No args." },
#endif
#ifdef LOSCFG_DEBUG_SEMAPHORE
    { "sem",        0, 0, OsCmdSem,        CMD_TYPE_EX, "Show semaphore info. No args." },
#endif
#ifdef LOSCFG_DEBUG_MUTEX
    { "mutex",      0, 0, OsCmdMutex,      CMD_TYPE_EX, "Show mutex info. No args." },
#endif
#ifdef LOSCFG_DEBUG_QUEUE
    { "queue",      0, 0, OsCmdQueue,      CMD_TYPE_EX, "Show queue info. No args." },
#endif
#ifdef LOSCFG_DEBUG_MEM_MWMD
    { "md",         2, 3, OsCmdMd,         CMD_TYPE_STD, "Memory dump. Usage: md addr [cnt] - addr read" },
    { "mw",         3, 4, OsCmdMw,         CMD_TYPE_STD, "Memory write. Usage: mw addr val [cnt] - addr write" },
#endif
#ifdef LOSCFG_MEM_LEAKCHECK
    { "memused",    0, 0, OsCmdMemUsed,    CMD_TYPE_EX, "Show current memory used nodes. Usage: memused" },
#endif
    { "free",       0, 1, OsCmdFree,       CMD_TYPE_EX, "Show memory usage or pool info. Usage: free [-k/-m]" },
    { "memcheck",   0, 0, OsCmdMemCheck,   CMD_TYPE_EX, "Check memory integrity. Usage: memcheck" },
#if (defined LOSCFG_ARCH_INTERRUPT_TAKEOVER) && (defined LOSCFG_DEBUG_HWI)
    { "hwi",        0, 3, OsCmdHwi,        CMD_TYPE_EX, "Show interrupt status. Usage: hwi status hwiNum [devId]" },
#endif
#ifdef LOSCFG_TRIGGER_ENABLE
    { "trigger",    0, 3, OsCmdTrigger,    CMD_TYPE_EX, "Protect address not be written.\
Usage: trigger [set] [addr] [size] or trigger [unset]" },
#endif
#ifdef LOSCFG_KERNEL_CPUP
    { "cpup",       0, 2, OsCmdCpup,       CMD_TYPE_EX, "Show Cpu info. Usage: cpup [MODE] cpup [MODE] [TASKID]" },
#endif
#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
    { "schedstatstart", 0, 0, OsCmdSchedStart, CMD_TYPE_EX, "Start scheduler statistics. No args." },
    { "schedstatstop",  0, 0, OsCmdSchedStop,  CMD_TYPE_EX, "Stop scheduler statistics and show summary. No args." },
    { "schedstatinfo",  0, 0, OsCmdSchedInfo,  CMD_TYPE_EX, "Show detailed per-task scheduler statistics. No args." },
#endif
#ifdef LOSCFG_KERNEL_PERF
    { "perfstart",  0, 0, OsCmdPerfStart,  CMD_TYPE_EX, "Start hardware PMU sampling. No args." },
    { "perfstop",   0, 0, OsCmdPerfStop,   CMD_TYPE_EX, "Stop hardware PMU sampling. No args." },
    { "perfdump",   0, 0, OsCmdPerfDump,   CMD_TYPE_EX, "Dump perf data and deinit. No args." },
#endif
#ifdef LOSCFG_MEMFRAG_DFX_SHOW_INFO
    { "memfraginfo",  0, 0, OsCmdMemFragInfo,  CMD_TYPE_EX, "Count the number of memory fragments. No args." },
#endif

};

#define OS_CMD_COUNT (sizeof(g_osCmdTable)/sizeof(g_osCmdTable[0]))

#define OS_CMD_MAX_TOKENS 32
#define OS_CMD_MAX_INPUT  256

STATIC INLINE CHAR *TokenSkipSpaces(CHAR *p)
{
    if (p == NULL) {
        return NULL;
    }
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}
/* Parse a quoted parameter and returns the position after parsing ends (which could be '\0') */
STATIC INLINE CHAR *ParseQuoted(CHAR *p)
{
    if (p == NULL) {
        return NULL;
    }
    // p points to '"'
    p++;
    while (*p && *p != '\"') {
        p++;
    }
    if (*p == '\"') {
        *p = '\0'; // Terminate the string
        return p + 1;
    }
    // Closing quote not found, p is already at '\0'
    return p;
}

/* Parse an unquoted parameter and returns the position after parsing ends (which could be '\0') */
STATIC INLINE CHAR *ParseUnquoted(CHAR *p)
{
    if (p == NULL) {
        return NULL;
    }
    while (*p && !isspace((unsigned char)*p)) {
        p++;
    }
    if (*p) {
        *p = '\0';
        return p + 1;
    }
    return p;
}

STATIC INT32 Tokenize(const CHAR *cmdline, CHAR *buf, size_t bufSize, const CHAR **argv, INT32 maxArgv)
{
    if (cmdline == NULL || buf == NULL || argv == NULL || maxArgv <= 0) {
        return TOKENIZE_ERR_INVALID_ARGS;
    }

    size_t inLen = strnlen(cmdline, OS_CMD_MAX_INPUT);
    if (inLen >= bufSize) {
        return TOKENIZE_ERR_INPUT_TOO_LONG;
    }

    INT32 ret = memcpy_s(buf, bufSize, cmdline, inLen + 1);
    if (ret != EOK) {
        PRINTK("[%s, %d] memcpy_s failed errno:%d.\n", __FUNCTION__, __LINE__, ret);
        return TOKENIZE_ERR_INVALID_ARGS;
    }

    INT32 argc = 0;
    CHAR *p = buf;

    p = TokenSkipSpaces(p);
    while (*p != '\0') {
        if (argc >= maxArgv) {
            return TOKENIZE_ERR_TOO_MANY_ARGS;
        }

        CHAR *start;
        if (*p == '\"') {
            start = p + 1;
            p = ParseQuoted(p);
        } else {
            start = p;
            p = ParseUnquoted(p);
        }

        argv[argc++] = start;
        p = TokenSkipSpaces(p);
    }

    return argc;
}

STATIC const LosCmdEntry *FindCmd(const CHAR *name)
{
    if (name == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < OS_CMD_COUNT; ++i) {
        if (strcmp(name, g_osCmdTable[i].name) == 0) {
            return &g_osCmdTable[i];
        }
    }
    return NULL;
}

STATIC VOID PrintHelp(VOID)
{
    PRINTK("Available commands:\n");
    for (size_t i = 0; i < OS_CMD_COUNT; ++i) {
        PRINTK("%16s  (argc: %u-%u)  - %s\n",
               g_osCmdTable[i].name,
               g_osCmdTable[i].min_argc,
               g_osCmdTable[i].max_argc,
               (g_osCmdTable[i].help != NULL) ? g_osCmdTable[i].help : "");
    }
}

INT32 OSExecUserCmd(const CHAR *cmdline)
{
    if (cmdline == NULL) {
        PRINTK("ExecUserCmd: cmdline is NULL\n");
        return TOKENIZE_ERR_INVALID_ARGS;
    }

    CHAR buf[OS_CMD_MAX_INPUT];
    const CHAR *argv[OS_CMD_MAX_TOKENS];

    INT32 tokc = Tokenize(cmdline, buf, sizeof(buf), argv, OS_CMD_MAX_TOKENS);
    if (tokc < 0) {
        if (tokc == TOKENIZE_ERR_INPUT_TOO_LONG) {
            PRINTK("ExecUserCmd: cmdline too long (max %u)\n", (unsigned)sizeof(buf)-1);
        } else if (tokc == TOKENIZE_ERR_TOO_MANY_ARGS) {
            PRINTK("ExecUserCmd: too many arguments (max %d)\n", OS_CMD_MAX_TOKENS - 1);
        } else {
            PRINTK("ExecUserCmd: tokenize error %d\n", tokc);
        }
        return TOKENIZE_ERR_INVALID_ARGS;
    }

    if (tokc == 0) {
        PRINTK("ExecUserCmd: no command\n");
        return TOKENIZE_ERR_INVALID_ARGS;
    }

    INT32 argc;
    const CHAR **args;
    const CHAR *cmdName = argv[0];
    const LosCmdEntry *ent = FindCmd(cmdName);
    if (ent == NULL) {
        PRINTK("Unknown command: %s\n", cmdName);
        PrintHelp();
        return TOKENIZE_ERR_INVALID_ARGS;
    }
    if (ent->cmdType == CMD_TYPE_EX) {
        argc = tokc - 1;
        args = &argv[1];
    }
    if (ent->cmdType == CMD_TYPE_STD) {
        argc = tokc;
        args = &argv[0];
    }

    if ((unsigned)argc < ent->min_argc || (unsigned)argc > ent->max_argc) {
        PRINTK("Usage error: %s expects %u-%u args, got %d\n",
               ent->name, ent->min_argc, ent->max_argc, argc);
        if (ent->help) {
            PRINTK("  Help: %s\n", ent->help);
        }
        return TOKENIZE_ERR_INVALID_ARGS;
    }
    INT32 ret = ent->func(argc, args);
    return ret;
}
