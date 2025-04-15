#pragma once
#include "core/buffer.hpp"
#include "core/editor.hpp"
#include "ui/winConsole.hpp"

class EditorState;

void renderBuffer(Buffer& buf, EditorState& editor);

void renderStatusBar(EditorState& editor);

void renderCursor(EditorState& editor);
