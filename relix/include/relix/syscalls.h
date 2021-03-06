#ifndef RELIX_SYSCALLS_H
#define RELIX_SYSCALLS_H


#define __NR_restart_syscall             0
#define __NR__exit                       1
#define __NR_fork                        2
#define __NR_read                        3
#define __NR_write                       4
#define __NR_openat                      5  // open
#define __NR_close                       6
#define __NR_waitpid                     7
#define __NR_vfork_start                 8  // creat
#define __NR_linkat                      9  // link
#define __NR_unlinkat                   10  // unlink
#define __NR_execve                     11
#define __NR_chdir                      12
#define __NR_time                       13
#define __NR_mknodat                    14  // mknod
#define __NR_fchmodat                   15  // chmod
#define __NR_fchownat                   16  // lchown
#define __NR_reexec                     17
#define __NR_fstatat                    18  // stat
#define __NR_lseek                      19
#define __NR_getpid                     20
#define __NR_mount                      21
#define __NR_umount                     22
#define __NR_setuid                     23
#define __NR_getuid                     24
#define __NR_stime                      25
#define __NR_ptrace                     26
#define __NR_alarm                      27
#define __NR_fstat                      28
#define __NR_pause                      29
#define __NR_utimensat                  30  // utime
// 31
// 32
#define __NR_faccessat                  33  // access
#define __NR_nice                       34
#define __NR_ftime                      35
#define __NR_sync                       36
#define __NR_kill                       37
#define __NR_renameat                   38  // rename
#define __NR_mkdirat                    39  // mkdir
//#define __NR_rmdir                      40
#define __NR_dup3                       41  // dup
#define __NR_pipe2                      42  // pipe
#define __NR_times                      43
#define __NR_prof                       44
#define __NR_brk                        45
#define __NR_setgid                     46
#define __NR_getgid                     47
#define __NR_signal                     48
#define __NR_geteuid                    49
#define __NR_getegid                    50
#define __NR_acct                       51
#define __NR_umount2                    52
// 53
#define __NR_ioctl                      54
#define __NR_fcntl                      55
#define __NR_ttypair                    56  // mpx
#define __NR_setpgid                    57
// 58
#define __NR_uname                      59
#define __NR_umask                      60
#define __NR_chroot                     61
#define __NR_ustat                      62
#define __NR_fork_and_exit              63  // dup2
#define __NR_getppid                    64
#define __NR_gettid                     65  // getpgrp
#define __NR_setsid                     66
#define __NR_sigaction                  67
#define __NR_sgetmask                   68
#define __NR_ssetmask                   69
#define __NR_setreuid                   70
#define __NR_setregid                   71
#define __NR_sigsuspend                 72
#define __NR_sigpending                 73
#define __NR_sethostname                74
#define __NR_setrlimit                  75
#define __NR_getrlimit                  76
#define __NR_getrusage                  77
#define __NR_gettimeofday               78
#define __NR_settimeofday               79
// 80
// 81
#define __NR_select                     82
#define __NR_symlinkat                  83  // symlink
#define __NR__realpathat                84  // stat
#define __NR__readlinkat                85  // readlink
// 86
// 87
// 88
// 89
#define __NR__relix_mmap                90
#define __NR_munmap                     91
#define __NR_truncate                   92
#define __NR_ftruncate                  93
#define __NR_fchmod                     94
#define __NR_fchown                     95
// 96
// 97
// 98
// 99
// 100
// 101
// 102
// 103
#define __NR_setitimer                 104
#define __NR_getitimer                 105
#define __NR_copyfileat                106  // stat
#define __NR_updateat                  107  // lstat
//#define __NR_fstat                     108
#define __NR__OTInetStringToAddress    109  // uname
// 110
// 111
// 112
// 113
// 114
// 115
// 116
#define __NR_AESendBlocking            117  // ipc
#define __NR_fsync                     118
// 119
#define __NR__relix_clone              120  // clone
#define __NR__OTInetMailExchange       121  // setdomainname
#define __NR_unshare                   122  // uname
// 123
// 124
// 125
#define __NR_sigprocmask               126
// 127
// 128
// 129
// 130
// 131
#define __NR_getpgid                   132
#define __NR_fchdir                    133
// 134
// 135
// 136
// 137
// 138
// 139
// 140
#define __NR_getdents                  141
// 142
#define __NR_flock                     143
#define __NR_msync                     144
#define __NR_readv                     145
#define __NR_writev                    146
#define __NR_getsid                    147
#define __NR_fdatasync                 148
// 149
// 150
// 151
// 152
// 153
// 154
// 155
// 156
// 157
// 158
// 159
// 160
// 161
#define __NR_nanosleep                 162
// 163
// 164
// 165
// 166
// 167
#define __NR_poll                      168

// ...

#define __NR_pread                     180
#define __NR_pwrite                    181

// ...

#define __NR_pump                      187  // sendfile
// 188  // getpmsg
// 189  // putpmsg
#define __NR_vfork                     190

// ...

#define __NR_socketpair                191
#define __NR_socket                    192
#define __NR_bind                      193
#define __NR_listen                    194
#define __NR_accept                    195
#define __NR_connect                   196
#define __NR_shutdown                  197
#define __NR_getsockname               198
#define __NR_getpeername               199
#define __NR_getsockopt                200
#define __NR_setsockopt                201
#define __NR_send                      202
#define __NR_recv                      203
#define __NR_sendto                    204
#define __NR_recvfrom                  205
#define __NR_sendmsg                   206
#define __NR_recvmsg                   207

#define __NR_preadv                    333
#define __NR_pwritev                   334

#endif
