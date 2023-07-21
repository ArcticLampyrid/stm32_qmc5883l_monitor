#pragma once
#ifdef __cplusplus
// C++-only part
extern "C"
{
#endif
    // C-compatible part
    void debug_log(const char *format, ...);
#ifdef __cplusplus
}
#endif