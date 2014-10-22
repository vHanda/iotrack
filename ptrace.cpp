/*
 * Copyright (C) 2014  Vishesh Handa <vhanda@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "ptrace.h"

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/unistd.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/user.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>

PTrace::PTrace(const std::string& exe, const std::vector<std::string>& args)
    : m_exe(exe)
    , m_args(args)
{
}

PTrace::~PTrace()
{

}

void PTrace::exec()
{
    pid_t child = fork();
    if (child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        char* args[100];
        args[0] = const_cast<char*>(m_exe.c_str());
        args[1] = 0;
        for (uint i = 0; i < m_args.size(); i++) {
            args[i + 1] = const_cast<char*>(m_args[i].c_str());
            args[i + 2] = 0;
        }
        execvp(m_exe.c_str(), args);
        return;
    }

    bool inSysCall = false;
    int fd = -1;
    int size = -1;
    std::string filePath;

    while (1) {
        int status;
        wait(&status);

        if (WIFEXITED(status)) {
            break;
        }

        long orig_rax = ptrace(PTRACE_PEEKUSER, child, 8 * ORIG_RAX, NULL);
        if (orig_rax == SYS_write) {
            if (!inSysCall) {
                user_regs_struct regs;
                ptrace(PTRACE_GETREGS, child, NULL, &regs);
                fd = regs.rdi;
                size = regs.rdx;

                inSysCall = true;
            }
            else {
                int ret = ptrace(PTRACE_PEEKUSER, child, 8 * RAX, NULL);
                inSysCall = false;
                handleWrite(ret, fd, size);
            }
        }
        else if (orig_rax == SYS_read) {
            if (!inSysCall) {
                user_regs_struct regs;
                ptrace(PTRACE_GETREGS, child, NULL, &regs);
                fd = regs.rdi;
                size = regs.rdx;

                inSysCall = true;
            }
            else {
                int ret = ptrace(PTRACE_PEEKUSER, child, 8 * RAX, NULL);
                inSysCall = false;
                handleRead(ret, fd, size);
            }
        }
        else if (orig_rax == SYS_pwrite64) {
            // TODO: handle pwrite

        }
        else if (orig_rax == SYS_pread64) {
            // TODO: handle pread
        }
        else if (orig_rax == SYS_open) {
            if (!inSysCall) {
                user_regs_struct regs;
                ptrace(PTRACE_GETREGS, child, NULL, &regs);

                long charAddr = regs.rdi;
                filePath.clear();
                while (1) {
                    long word = ptrace(PTRACE_PEEKTEXT, child, charAddr, NULL);
                    char str[9];
                    strncpy(str, (char*)&word, 8);

                    bool stringEnded = false;
                    for (int i = 0; i < 8; i++) {
                        if (!str[i]) {
                            stringEnded = true;
                            break;
                        }
                    }
                    if (stringEnded)
                        break;
                    str[8] = 0;

                    filePath.append(str);
                    charAddr += 8;
                }
                inSysCall = true;
            }
            else {
                long rax = ptrace(PTRACE_PEEKUSER, child, 8 * RAX, NULL);
                inSysCall = false;

                handleOpen(rax, filePath);
            }

        }
        else if (orig_rax == SYS_close) {
            if (!inSysCall) {
                user_regs_struct regs;
                ptrace(PTRACE_GETREGS, child, NULL, &regs);

                fd = regs.rdi;
                inSysCall = true;
            }
            else {
                inSysCall = false;
                handleClose(fd);
            }
        }

        ptrace(PTRACE_SYSCALL, child, NULL, NULL);
    }
    ptrace(PTRACE_CONT, child, NULL, NULL);
}
