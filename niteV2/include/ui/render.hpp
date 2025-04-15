#ifndef RENDER_HPP
#define RENDER_HPP
#include "core/buffer.hpp"
#include "core/editor.hpp"

void renderBuffer(Buffer& buf, EditorState& editor);

void renderStatusBar(EditorState& editor);

void renderCursor(EditorState& editor);

#endif // RENDER_HPP
