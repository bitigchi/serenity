/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Vector.h>
#include <LibCore/ArgsParser.h>
#include <LibCore/System.h>
#include <LibMain/Main.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    TRY(Core::System::pledge("stdio rpath", nullptr));

    Vector<String> paths;

    Core::ArgsParser args_parser;
    args_parser.set_general_help("Concatenate files or pipes to stdout.");
    args_parser.add_positional_argument(paths, "File path", "path", Core::ArgsParser::Required::No);
    args_parser.parse(arguments);

    Vector<int> fds;
    if (!paths.is_empty()) {
        for (auto const& path : paths) {
            int fd;
            if (path == "-") {
                fd = 0;
            } else if ((fd = open(path.characters(), O_RDONLY)) == -1) {
                warnln("Failed to open {}: {}", path, strerror(errno));
                continue;
            }
            fds.append(fd);
        }
    } else {
        fds.append(0);
    }

    TRY(Core::System::pledge("stdio", nullptr));

    for (auto& fd : fds) {
        for (;;) {
            char buf[32768];
            ssize_t nread = read(fd, buf, sizeof(buf));
            if (nread == 0)
                break;
            if (nread < 0) {
                perror("read");
                return 2;
            }
            size_t already_written = 0;
            while (already_written < (size_t)nread) {
                ssize_t nwritten = write(1, buf + already_written, nread - already_written);
                if (nwritten < 0) {
                    perror("write");
                    return 3;
                }
                already_written += nwritten;
            }
        }
        close(fd);
    }

    return 0;
}
