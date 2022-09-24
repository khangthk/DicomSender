#pragma once

#include "global.h"

#include <QString>

class DICOMBASE_EXPORT DcmtkHelper
{
public:
    static bool isDicom(const QString &file);
};
