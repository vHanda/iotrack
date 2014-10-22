/*
 * Copyright 2014 Vishesh Handa <vhanda@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */


#include "ptrace.h"

#include <iostream>

class Trace : public PTrace
{
public:
    Trace(const std::string& exe, const std::vector<std::string>& args)
        : PTrace(exe, args)
    {
    }

protected:
    virtual void handleOpen(int ret, const std::string& path) {
        std::cout << "OPEN " << ret << " " << path << "\n";
    }

    virtual void handleClose(int fd) {
        std::cout << "CLOSE " << fd << "\n";
    }

    virtual void handleRead(int ret, int fd, int size) {
        std::cout << "READ " << ret << " " << fd << " " << size << "\n";
    }

    virtual void handleWrite(int ret, int fd, int size) {
        std::cout << "WRITE " << ret << " " << fd << " " << size << "\n";

        auto bt = fetchBacktrace();
        for (auto b : bt) {
            std::cout << "  " << b.ip << " " << b.offset << " " << b.name << "\n";
        }
    }
};

int main(int, char**)
{
    std::vector<std::string> args;
    args.push_back("-l");

    Trace trace("ls", args);
    trace.exec();

    return 0;
}
