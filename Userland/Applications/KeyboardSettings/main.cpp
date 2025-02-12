/*
 * Copyright (c) 2020, Hüseyin Aslıtürk <asliturk@hotmail.com>
 * Copyright (c) 2021, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "KeyboardSettingsWidget.h"
#include <LibCore/System.h>
#include <LibGUI/Application.h>
#include <LibGUI/SettingsWindow.h>
#include <LibGUI/WindowServerConnection.h>
#include <LibMain/Main.h>

// Including this after to avoid LibIPC errors
#include <LibConfig/Client.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    TRY(Core::System::pledge("stdio rpath cpath wpath recvfd sendfd unix proc exec", nullptr));
    auto app = TRY(GUI::Application::try_create(arguments));
    Config::pledge_domains("KeyboardSettings");

    TRY(Core::System::pledge("stdio rpath cpath wpath recvfd sendfd proc exec", nullptr));
    TRY(Core::System::unveil("/res", "r"));
    TRY(Core::System::unveil("/bin/keymap", "x"));
    TRY(Core::System::unveil("/proc/keymap", "r"));
    TRY(Core::System::unveil(nullptr, nullptr));

    auto app_icon = GUI::Icon::default_icon("app-keyboard-settings");

    auto window = TRY(GUI::SettingsWindow::try_create("Keyboard Settings"));
    window->set_icon(app_icon.bitmap_for_size(16));
    window->add_tab<KeyboardSettingsWidget>("Keyboard");

    window->show();
    return app->exec();
}
