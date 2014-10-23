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
#include <fstream>
#include <unordered_map>

using namespace std;

class Trace : public PTrace
{
public:
    Trace(const string& exe, const vector<string>& args,
          const string& outputFile)
        : PTrace(exe, args)
        , m_outputFile(outputFile)
    {
        m_ostream.open(m_outputFile, ofstream::out | ofstream::trunc);
    }

protected:
    virtual void handleOpen(int ret, const string& path) {
        int id = stringId(path);
        m_ostream << "o " << ret << " " << id << "\n";
    }

    virtual void handleClose(int fd) {
        m_ostream << "c " << fd << "\n";
    }

    virtual void handleRead(int ret, int fd, int size) {
        m_ostream << "r " << ret << " " << fd << " " << size << "\n";
        printBacktrace();
    }

    virtual void handleWrite(int ret, int fd, int size) {
        m_ostream << "w " << ret << " " << fd << " " << size << "\n";
        printBacktrace();
    }

private:
    int stringId(const string& str) {
        auto it = m_strings.find(str);
        if (it == m_strings.end()) {
            int id = m_strings.size();
            m_strings.insert(make_pair(str, id));
            m_ostream << "s " << id << " " << str << "\n";
            return id;
        }

        return it->second;
    }

    void printBacktrace() {
        auto bt = fetchBacktrace();

        vector<int> backtraceIds;
        for (const auto& b : bt) {
            string s = to_string(b.ip) + " " + b.name;
            int id = stringId(s);
            backtraceIds.push_back(id);
        }

        m_ostream << "bt ";
        for (int id : backtraceIds) {
            m_ostream << id << " ";
        }
        m_ostream << "\n";
    }

    string m_outputFile;
    ofstream m_ostream;

    unordered_map<string, int> m_strings;
};

int main(int, char**)
{
    std::vector<std::string> args;
    args.push_back("fire");

    Trace trace("baloosearch", args, "iotrace.output");
    trace.exec();

    return 0;
}
