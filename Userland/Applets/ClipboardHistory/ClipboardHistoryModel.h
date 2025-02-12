/*
 * Copyright (c) 2019-2020, Sergey Bugaev <bugaevc@serenityos.org>
 * Copyright (c) 2021, Mustafa Quraish <mustafa@cs.toronto.edu>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Vector.h>
#include <LibConfig/Listener.h>
#include <LibGUI/Clipboard.h>
#include <LibGUI/Model.h>

class ClipboardHistoryModel final : public GUI::Model
    , public GUI::Clipboard::ClipboardClient
    , public Config::Listener {
public:
    static NonnullRefPtr<ClipboardHistoryModel> create();

    enum Column {
        Data,
        Type,
        Size,
        __Count
    };

    virtual ~ClipboardHistoryModel() override;

    const GUI::Clipboard::DataAndType& item_at(int index) const { return m_history_items[index]; }
    void remove_item(int index);

    // ^Config::Listener
    virtual void config_string_did_change(String const& domain, String const& group, String const& key, String const& value) override;

private:
    ClipboardHistoryModel();
    void add_item(const GUI::Clipboard::DataAndType& item);

    // ^GUI::Model
    virtual int row_count(const GUI::ModelIndex&) const override { return m_history_items.size(); }
    virtual String column_name(int) const override;
    virtual int column_count(const GUI::ModelIndex&) const override { return Column::__Count; }
    virtual GUI::Variant data(const GUI::ModelIndex&, GUI::ModelRole) const override;

    // ^GUI::Clipboard::ClipboardClient
    virtual void clipboard_content_did_change(const String&) override { add_item(GUI::Clipboard::the().fetch_data_and_type()); }

    Vector<GUI::Clipboard::DataAndType> m_history_items;
    size_t m_history_limit;
};
