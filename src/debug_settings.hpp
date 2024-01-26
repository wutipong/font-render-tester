#ifndef DEBUG_SETTINGS_HPP
#define DEBUG_SETTINGS_HPP

struct DebugSettings {
  bool enabled{false};
  bool debugGlyphBound{true};
  bool debugBaseline{true};
  bool debugCaret{true};
  bool debugAscend{true};
  bool debugDescend{true};
};
#endif
