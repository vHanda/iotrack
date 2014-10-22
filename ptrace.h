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

#ifndef PTRACE_H
#define PTRACE_H

#include <string>
#include <vector>

#include <libunwind.h>
#include <libunwind-ptrace.h>

class PTrace
{
public:
    PTrace(const std::string& exe, const std::vector<std::string>& args);
    virtual ~PTrace();

    void exec();

protected:
    virtual void handleWrite(int ret, int fd, int size) = 0;
    virtual void handleRead(int ret, int fd, int size) = 0;
    virtual void handleOpen(int ret, const std::string& path) = 0;
    virtual void handleClose(int fd) = 0;

    struct Backtrace {
        uint64_t ip;
        uint64_t offset;
        char name[512];
    };
    std::vector<Backtrace> fetchBacktrace();

private:
    std::string m_exe;
    std::vector<std::string> m_args;

    pid_t m_child;
    unw_addr_space_t m_unw_as;
};

#endif // PTRACE_H
