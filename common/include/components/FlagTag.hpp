#pragma once

struct FlagTag {
    bool captured = false;

    FlagTag(bool captured = false) : captured(false) {}
};