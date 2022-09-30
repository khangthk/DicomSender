#include "gdcmhelper.h"

#include <gdcmReader.h>
#include <gdcmImageReader.h>

using namespace gdcm;

bool GdcmHelper::isDicom(const QString &file)
{
    Reader reader;
    reader.SetFileName(qPrintable(file));
    return reader.Read();
}

bool GdcmHelper::isImage(const QString &file)
{
    ImageReader reader;
    reader.SetFileName(qPrintable(file));
    return reader.Read();
}
