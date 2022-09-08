#pragma once

#include "global.h"

#include <QString>

class DICOMBASE_EXPORT GdcmHelper
{
public:
    static bool isDicom(const QString &file);
    static bool isImage(const QString &file);
};
