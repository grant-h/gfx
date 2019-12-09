#include <DebugEditor.hpp>

const TextEditor::Palette custom_palette = { {
    0xffafafaf,	// Default
    0xffd69c56,	// Keyword	
    0xff00ff00,	// Number
    0xff7070e0,	// String
    0xff70a0e0, // Char literal
    0xffffffff, // Punctuation
    0xff408080,	// Preprocessor
    0xffaaaaaa, // Identifier
    0xff9bc64d, // Known identifier
    0xffc040a0, // Preproc identifier
    0xff206020, // Comment (single line)
    0xff406020, // Comment (multi line)
    0x00101010, // Background
    0xffe0e0e0, // Cursor
    0x80a06020, // Selection
    0x800020ff, // ErrorMarker
    0x40f08000, // Breakpoint
    0xff707000, // Line number
    0x40000000, // Current line fill
    0x40808080, // Current line fill (inactive)
    0x40a0a0a0, // Current line edge
} };
