#pragma once

enum class RegisterType
{
    None,
    FmAr,   // AR (0-31)
    FmDr,   // DR (0-31)
    FmSr,   // SR (0-31)
    FmRr,   // RR (0-15)
    FmSl,   // SL (0-15)
    FmTl,   // TL (0-127)
    FmMul,  // MUL (0-15)
    FmDt,   // DT (0-7)
    FmDt2,  // DT2 (0-3)
    SsgVol, // Level (0-15)
    SsgEnv  // Env Period (0-65535)
};
